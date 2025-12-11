using System;
using System.Threading;

namespace strategy;

public class Fork
{
    public enum State : byte
    {
        AVAILABLE,
        IN_USE
    }

    private readonly object sync = new();
    private State state;
    private string? usedBy;
    private readonly int id;
    private DateTime lastStateChange;
    private TimeSpan availableTime;
    private TimeSpan inUseTime;

    public Fork(int id)
    {
        this.id = id;
        state = State.AVAILABLE;
        usedBy = null;
        lastStateChange = DateTime.UtcNow;
        availableTime = TimeSpan.Zero;
        inUseTime = TimeSpan.Zero;
    }

    public int Id => id;

    public string GetState()
    {
        lock (sync)
        {
            return state.ToString();
        }
    }

    public string UsedBy()
    {
        lock (sync)
        {
            return usedBy ?? "";
        }
    }

    public bool TryTake(string philosopherName, int acquisitionDelayMs, CancellationToken token)
    {
        // Optimistic short wait to avoid long blocking and reduce deadlocks.
        if (!Monitor.TryEnter(sync))
        {
            LockTracker.RegisterWaiting(philosopherName, id);
            return false;
        }

        try
        {
            if (state != State.AVAILABLE)
            {
                LockTracker.RegisterWaiting(philosopherName, id);
                return false;
            }

            UpdateDurations(State.IN_USE);
            usedBy = philosopherName;
            state = State.IN_USE;
            LockTracker.RegisterAcquired(philosopherName, id);
            LockTracker.ClearWaiting(philosopherName, id);
        }
        finally
        {
            Monitor.Exit(sync);
        }

        // Simulate the time it takes to grab the fork.
        SleepWithCancellation(acquisitionDelayMs, token);
        return !token.IsCancellationRequested;
    }

    public void Release(string philosopherName)
    {
        lock (sync)
        {
            if (state == State.IN_USE && usedBy == philosopherName)
            {
                UpdateDurations(State.AVAILABLE);
                state = State.AVAILABLE;
                usedBy = null;
                LockTracker.RegisterReleased(philosopherName, id);
            }
        }
    }

    public (double availablePct, double inUsePct) GetUtilization(TimeSpan totalElapsed)
    {
        lock (sync)
        {
            // Update counters up to now before reporting.
            UpdateDurations(state);
            var total = availableTime + inUseTime;
            if (totalElapsed > total)
            {
                total = totalElapsed;
            }
            double availablePct = total.TotalMilliseconds > 0 ? availableTime.TotalMilliseconds / total.TotalMilliseconds * 100.0 : 0.0;
            double inUsePct = total.TotalMilliseconds > 0 ? inUseTime.TotalMilliseconds / total.TotalMilliseconds * 100.0 : 0.0;
            return (availablePct, inUsePct);
        }
    }

    private void UpdateDurations(State nextState)
    {
        var now = DateTime.UtcNow;
        var delta = now - lastStateChange;
        if (state == State.AVAILABLE)
        {
            availableTime += delta;
        }
        else
        {
            inUseTime += delta;
        }
        lastStateChange = now;
        state = nextState;
    }

    private static void SleepWithCancellation(int ms, CancellationToken token)
    {
        const int slice = 5;
        int waited = 0;
        while (waited < ms && !token.IsCancellationRequested)
        {
            int portion = Math.Min(slice, ms - waited);
            Thread.Sleep(portion);
            waited += portion;
        }
    }
}