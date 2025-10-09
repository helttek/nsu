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
    private readonly Fork right;
    private readonly Fork left;

    public Philosopher(string name, ref Fork left, ref Fork right, Strategy strategy)
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
        this.left = left;
        this.right = right;
    }

    public Philosopher(string name, ref Fork left, ref Fork right)
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
        this.left = left;
        this.right = right;
    }

    public void MakeMove()
    {
        step++;
        switch (state)
        {
            case State.Hungry:
                FinishedHungry();
                break;

            case State.TakingFork:
                if (step >= stepsToTakeFork)
                {
                    FinishedTakingFork();
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

            case State.Thinking:
                if (step >= stepsToThink)
                {
                    FinishedThinking();
                    return;
                }
                break;

            default:
                throw new Exception("Error: " + name + " has unknown state");
        }
    }

    private void ReleaseForks()
    {
        left.Release();
        right.Release();
    }

    public void TakeLeftFork()
    {

    }

    public void TakeRightFork() { }

    private void FinishedThinking()
    {
        step = 0;
        stepsToThink = (uint)random.Next(minStepsToThink, maxStepsToThink + 1);
        state = State.Hungry;
    }

    private void FinishedHungry()
    {
        if (left == null)
        {
            throw new Exception("ERROR " + name + ": left fork is null (wtf)");
        }
        if (right == null)
        {
            throw new Exception("ERROR " + name + ": right fork is null (wtf)");
        }
        string? firstMove = strategy?.NextMove();
        if (firstMove == null) { throw new Exception(name + ": Strategy is null"); }
        if (firstMove == "left")
        {
            try
            {
                left.Take(name);
                try
                {
                    right.Take(name);
                }
                catch (System.Exception e)
                {
                    state = State.Hungry;
                    step = 0;
                    left.Release();
                    Console.Error.WriteLine(name + ": failed to take the right fork: " + e.Message);
                    return;
                }
            }
            catch (System.Exception e)
            {
                state = State.Hungry;
                step = 0;
                Console.Error.WriteLine(name + ": failed to take the left fork: " + e.Message);
                return;
            }
        }
        else
        {
            try
            {
                right.Take(name);
                try
                {
                    left.Take(name);
                }
                catch (System.Exception e)
                {
                    state = State.Hungry;
                    step = 0;
                    right.Release();
                    Console.Error.WriteLine(name + ": failed to take the left fork: " + e.Message);
                    return;
                }
            }
            catch (System.Exception e)
            {
                state = State.Hungry;
                step = 0;
                Console.Error.WriteLine(name + ": failed to take the right fork: " + e.Message);
                return;
            }
        }
        step = 0;
        state = State.TakingFork;
        return;
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
        state = State.Eating;
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