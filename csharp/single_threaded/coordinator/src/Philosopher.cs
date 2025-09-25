namespace coordinator;

using strategy;

public class Philosopher
{
    private enum State : byte
    {
        Thinking,
        Hungry,
        Eating,
        TakingFork
    }
    private const int maxStepsToEat = 5;
    private const int minStepsToEat = 4;
    private const int maxStepsToThink = 10;
    private const int minStepsToThink = 3;
    private Random random;
    private uint step;
    private uint stepsToEat;
    private uint stepsToThink;
    private uint stepsToTakeFork;

    private string name { get; }

    private State state;

    private Strategy? strategy;

    public Philosopher(string name, Strategy strategy)
    {
        this.state = State.Hungry;
        this.name = name;
        this.strategy = strategy;
        this.step = 0;
        this.random = new Random();
        this.stepsToEat = (uint)random.Next(minStepsToEat, maxStepsToEat + 1);
        this.stepsToThink = (uint)random.Next(minStepsToThink, maxStepsToThink + 1);
        this.stepsToTakeFork = 2;
    }

    public Philosopher(string name)
    {
        this.state = State.Hungry;
        this.name = name;
        this.strategy = null;
        this.step = 0;
        this.random = new Random();
        this.stepsToEat = (uint)random.Next(minStepsToEat, maxStepsToEat + 1);
        this.stepsToThink = (uint)random.Next(minStepsToThink, maxStepsToThink + 1);
        this.stepsToTakeFork = 2;
    }

    public void MakeMove()
    {
        if (strategy != null)
        {
            strategy.MakeMove();
        }
        step++;
        switch (state)
        {
            case State.Thinking:
                if (step >= stepsToThink)
                {
                    step = 1;
                    stepsToThink = (uint)random.Next(minStepsToThink, maxStepsToThink + 1);
                    state = State.Hungry;
                    return;
                }
                break;

            case State.Eating:
                if (step >= stepsToEat)
                {
                    step = 1;
                    stepsToEat = (uint)random.Next(minStepsToEat, maxStepsToEat + 1);
                    ReleaseForks();
                    state = State.Thinking;
                    return;
                }
                break;

            case State.TakingFork:
                if (step >= stepsToTakeFork)
                {
                    step = 1;
                    ReleaseForks();
                    state = State.Thinking;
                    return;
                }
                break;

            default:
                break;
        }
    }

    private void ReleaseForks() { }

    public void TakeLeftFork() { }

    public void TakeRightFork() { }

    public event Action<Philosopher>? IsHungry;
}