using System;
using System.Threading;

namespace strategy;

public class Philosopher
{
    public enum State : byte
    {
        THINKING,
        HUNGRY,
        EATING
    }

    private readonly string name;
    private readonly int index;
    private readonly Fork left;
    private readonly Fork right;
    private readonly Strategy strategy;
    private readonly CancellationToken token;
    private readonly Thread worker;
    private readonly Random random;
    private readonly object sync = new();
    private readonly int forkAcquireMs;
    private readonly int thinkMinMs;
    private readonly int thinkMaxMs;
    private readonly int eatMinMs;
    private readonly int eatMaxMs;

    private volatile State state;
    private string currentAction;
    private int eaten;
    private DateTime? hungerStart;
    private TimeSpan totalHungry;
    public Fork Left => left;
    public Fork Right => right;
    public Strategy Strategy => strategy;
    public int Index => index;
    public int ForkAcquireMs => forkAcquireMs;

    public Philosopher(string name, int index, Fork left, Fork right, Strategy strategy, CancellationToken token,
        int forkAcquireMs, int thinkMinMs, int thinkMaxMs, int eatMinMs, int eatMaxMs)
    {
        this.name = name;
        this.index = index;
        this.left = left;
        this.right = right;
        this.strategy = strategy;
        this.token = token;
        this.forkAcquireMs = forkAcquireMs;
        this.thinkMinMs = thinkMinMs;
        this.thinkMaxMs = thinkMaxMs;
        this.eatMinMs = eatMinMs;
        this.eatMaxMs = eatMaxMs;
        random = new Random(Environment.TickCount + index);
        state = State.THINKING;
        currentAction = "Thinking";
        worker = new Thread(Run) { IsBackground = true, Name = $"Philosopher-{name}" };
    }

    public void Start() => worker.Start();
    public void Join() => worker.Join();
    public string GetName() => name;
    public State GetState() => state;
    public int GetEaten() { lock (sync) return eaten; }
    public string GetCurrentAction() { lock (sync) return currentAction; }

    public TimeSpan GetTotalHungry()
    {
        lock (sync)
        {
            if (state == State.HUNGRY && hungerStart.HasValue)
            {
                return totalHungry + (DateTime.UtcNow - hungerStart.Value);
            }
            return totalHungry;
        }
    }
    public void SetAction(string action)
    {
        lock (sync) currentAction = action;
    }

    private void Run()
    {
        try
        {
            while (!token.IsCancellationRequested)
            {
                DoThink();

                lock (sync)
                {
                    state = State.HUNGRY;
                    hungerStart = DateTime.UtcNow;
                    currentAction = "Hungry";
                }

                strategy.AcquireForks(this, token);

                DoEat();

                strategy.ReleaseForks(this);
            }
        }
        catch (OperationCanceledException)
        {
            // Ensure we don't leave forks locked if we shut down abruptly
            strategy.ReleaseForks(this);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error in philosopher {name}: {ex.Message}");
        }
    }

    private void DoThink()
    {
        lock(sync) state = State.THINKING;
        int duration = random.Next(thinkMinMs, thinkMaxMs + 1);
        SetAction($"Thinking ({duration}ms)");
        SleepWithCancellation(duration);
    }

    private void DoEat()
    {
        lock (sync)
        {
            if (hungerStart.HasValue)
            {
                totalHungry += DateTime.UtcNow - hungerStart.Value;
                hungerStart = null;
            }
            state = State.EATING;
        }

        int duration = random.Next(eatMinMs, eatMaxMs + 1);
        SetAction($"Eating ({duration}ms)");
        SleepWithCancellation(duration);

        lock (sync)
        {
            eaten++;
        }
    }

    private void SleepWithCancellation(int ms)
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