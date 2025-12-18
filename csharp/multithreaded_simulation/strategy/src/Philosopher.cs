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
        state = State.HUNGRY;
        currentAction = "Hungry";
        worker = new Thread(Run) { IsBackground = true, Name = $"Philosopher-{name}" };
    }

    public void Start() => worker.Start();

    public void Join() => worker.Join();

    public string GetName() => name;

    public State GetState() => state;

    public int GetEaten()
    {
        lock (sync)
        {
            return eaten;
        }
    }

    public string GetCurrentAction()
    {
        lock (sync)
        {
            return currentAction;
        }
    }

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

    private void Run()
    {
        while (!token.IsCancellationRequested)
        {
            switch (state)
            {
                case State.THINKING:
                    Think();
                    break;

                case State.HUNGRY:
                    TryAcquireForks();
                    break;

                case State.EATING:
                    Eat();
                    break;

                default:
                    throw new Exception("Invalid philosopher state: " + state + " for philosopher " + name);
            }
        }
    }

    private void Think()
    {
        int duration = random.Next(thinkMinMs, thinkMaxMs + 1);
        SetAction($"{duration} ms left");
        SleepWithCancellation(duration);
        if (token.IsCancellationRequested) return;

        lock (sync)
        {
            state = State.HUNGRY;
            hungerStart = DateTime.UtcNow;
            currentAction = "TakeRightFork|TakeLeftFork";
        }
    }

    private void TryAcquireForks()
    {
        bool leftFirst = strategy.TakeWhichFork(index);
        Fork first = leftFirst ? left : right;
        Fork second = leftFirst ? right : left;

        bool gotFirst = TakeFork(first, leftFirst ? "TakeLeftFork" : "TakeRightFork");
        if (!gotFirst)
        {
            Backoff();
            return;
        }

        bool gotSecond = TakeFork(second, leftFirst ? "TakeRightFork" : "TakeLeftFork");
        if (!gotSecond)
        {
            first.Release(name);
            SetAction("ReleaseFork");
            Backoff();
            return;
        }

        lock (sync)
        {
            if (hungerStart.HasValue)
            {
                totalHungry += DateTime.UtcNow - hungerStart.Value;
                hungerStart = null;
            }
            state = State.EATING;
        }
    }

    private bool TakeFork(Fork fork, string action)
    {
        SetAction($"{action} (+{forkAcquireMs} ms)");
        bool took = fork.TryTake(name, forkAcquireMs, token);
        return took && !token.IsCancellationRequested;
    }

    private void Eat()
    {
        int duration = random.Next(eatMinMs, eatMaxMs + 1);
        SetAction($"{duration} ms left");
        SleepWithCancellation(duration);

        left.Release(name);
        right.Release(name);

        lock (sync)
        {
            eaten++;
            state = State.THINKING;
            currentAction = "Thinking";
        }
    }

    private void Backoff()
    {
        int pause = random.Next(5, 15);
        SleepWithCancellation(pause);
    }

    private void SetAction(string action)
    {
        lock (sync)
        {
            currentAction = action;
        }
    }

    private void SleepWithCancellation(int ms)
    {
        const int slice = 10;
        int waited = 0;
        while (waited < ms && !token.IsCancellationRequested)
        {
            int portion = Math.Min(slice, ms - waited);
            Thread.Sleep(portion);
            waited += portion;
        }
    }
}