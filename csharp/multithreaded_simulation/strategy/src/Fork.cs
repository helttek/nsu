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
        lock (sync) return state.ToString();
    }

    public string UsedBy()
    {
        lock (sync) return usedBy ?? "";
    }

    public void Acquire(string philosopherName, int acquisitionDelayMs, CancellationToken token)
    {
        bool acquired = false;
        while (!acquired)
        {
            token.ThrowIfCancellationRequested();

            lock (sync)
            {
                if (state == State.AVAILABLE)
                {
                    state = State.IN_USE;
                    usedBy = philosopherName;
                    UpdateDurations(State.IN_USE);
                    acquired = true;
                }
            }

            if (!acquired)
            {
                Thread.Sleep(10);
            }
        }

        try
        {
            SleepWithCancellation(acquisitionDelayMs, token);
        }
        catch (OperationCanceledException)
        {
            Release(philosopherName);
            throw;
        }
    }

    public void Release(string philosopherName)
    {
        lock (sync)
        {
            if (usedBy == philosopherName)
            {
                usedBy = null;
                UpdateDurations(State.AVAILABLE);
                Monitor.PulseAll(sync);
            }
        }
    }

    public (double availablePct, double inUsePct) GetUtilization(TimeSpan totalElapsed)
    {
        lock (sync)
        {
            UpdateDurations(state);
            var total = availableTime + inUseTime;
            if (totalElapsed > total) total = totalElapsed;

            double totalMs = total.TotalMilliseconds;
            double availablePct = totalMs > 0 ? availableTime.TotalMilliseconds / totalMs * 100.0 : 0.0;
            double inUsePct = totalMs > 0 ? inUseTime.TotalMilliseconds / totalMs * 100.0 : 0.0;
            return (availablePct, inUsePct);
        }
    }

    private void UpdateDurations(State nextState)
    {
        var now = DateTime.UtcNow;
        var delta = now - lastStateChange;
        if (state == State.AVAILABLE) availableTime += delta;
        else if (state == State.IN_USE) inUseTime += delta;
        lastStateChange = now;
        state = nextState;
    }

    private static void SleepWithCancellation(int ms, CancellationToken token)
    {
        const int slice = 10;
        int waited = 0;
        while (waited < ms)
        {
            token.ThrowIfCancellationRequested();
            int portion = Math.Min(slice, ms - waited);
            Thread.Sleep(portion);
            waited += portion;
        }
    }
}