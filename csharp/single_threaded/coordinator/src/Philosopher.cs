namespace coordinator;

using strategy;

public class Philosopher
{
    public enum State : byte
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
    private uint eaten;

    private string name;

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
        this.eaten = 0;
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
        this.eaten = 0;
    }

    public void MakeMove()
    {
        step++;
        switch (state)
        {
            case State.Thinking:
                if (step >= stepsToThink)
                {
                    FinishedThinking();
                    return;
                }
                break;

            case State.Eating:
                if (step >= stepsToEat)
                {
                    FinishedEating();
                    return;
                }
                break;

            case State.TakingFork:
                if (step >= stepsToTakeFork)
                {
                    FinishedTakingFork();
                    return;
                }
                break;

            case State.Hungry:
                FinishedHungry();
                break;

            default:
                Console.WriteLine(name + ": unknown state");
                break;
        }
    }

    private void ReleaseForks() { }

    public void TakeLeftFork() { }

    public void TakeRightFork() { }

    private void FinishedThinking()
    {
        step = 0;
        stepsToThink = (uint)random.Next(minStepsToThink, maxStepsToThink + 1);
        state = State.Hungry;
    }

    private void FinishedHungry()
    {
        if (strategy != null)
        {
            strategy.MakeMove();
            return;
        }





    }

    private void FinishedEating()
    {
        eaten++;
        step = 0;
        stepsToEat = (uint)random.Next(minStepsToEat, maxStepsToEat + 1);
        ReleaseForks();
        state = State.Thinking;
    }

    private void FinishedTakingFork()
    {
        step = 0;
        ReleaseForks();
        state = State.Thinking;
    }

    public event Action<Philosopher>? IsHungry;

    public State GetState()
    {
        return state;
    }

    public string GetName()
    {
        return name;
    }

    public uint GetEaten()
    {
        return eaten;
    }

    public string GetCurrentAction()
    {
        return "some action/steps left";
    }
}