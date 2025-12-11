using System.Diagnostics;
using System.Threading;

namespace app;

public enum ForkStatus
{
    Available,
    InUse
}

public sealed class ForkState
{
    private readonly SemaphoreSlim gate = new(1, 1);
    private readonly object sync = new();
    private readonly Stopwatch stopwatch = Stopwatch.StartNew();

    private ForkStatus status = ForkStatus.Available;
    private string? usedBy;
    private TimeSpan availableTime = TimeSpan.Zero;
    private TimeSpan inUseTime = TimeSpan.Zero;
    private DateTime lastStateChange = DateTime.UtcNow;

    public ForkState(int id)
    {
        Id = id;
    }

    public int Id { get; }

    public ForkSnapshot GetSnapshot()
    {
        lock (sync)
        {
            UpdateDurations(status);
            return new ForkSnapshot(Id, status, usedBy, availableTime, inUseTime);
        }
    }

    public async Task<bool> TryTakeAsync(string philosopher, int acquisitionDelayMs, CancellationToken token)
    {
        if (!await gate.WaitAsync(0, token).ConfigureAwait(false))
        {
            return false;
        }

        lock (sync)
        {
            UpdateDurations(ForkStatus.InUse);
            status = ForkStatus.InUse;
            usedBy = philosopher;
        }

        try
        {
            await Task.Delay(acquisitionDelayMs, token).ConfigureAwait(false);
            if (token.IsCancellationRequested)
            {
                ReleaseInternal(philosopher);
                return false;
            }

            return true;
        }
        catch (OperationCanceledException)
        {
            ReleaseInternal(philosopher);
            return false;
        }
    }

    public void Release(string philosopher)
    {
        ReleaseInternal(philosopher);
    }

    private void UpdateDurations(ForkStatus next)
    {
        DateTime now = DateTime.UtcNow;
        TimeSpan delta = now - lastStateChange;
        if (status == ForkStatus.Available)
        {
            availableTime += delta;
        }
        else
        {
            inUseTime += delta;
        }

        lastStateChange = now;
        status = next;
    }

    private void ReleaseInternal(string philosopher)
    {
        lock (sync)
        {
            if (status == ForkStatus.InUse && usedBy == philosopher)
            {
                UpdateDurations(ForkStatus.Available);
                status = ForkStatus.Available;
                usedBy = null;
                gate.Release();
            }
        }
    }
}

public readonly record struct ForkSnapshot(
    int Id,
    ForkStatus Status,
    string? UsedBy,
    TimeSpan Available,
    TimeSpan InUse);

