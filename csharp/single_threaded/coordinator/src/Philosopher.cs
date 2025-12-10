namespace coordinator;

using System.Net;
using strategy;

public class Philosopher
{
    public enum State : byte
    {
        THINKING,
        HUNGRY,
        EATING,
        TAKING_FORK
    }
    private uint minStepsToEat;
    private uint maxStepsToEat;
    private uint minStepsToThink;
    private uint maxStepsToThink;
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
    private Coordinator coordinator;
    private bool leftTaken;
    private bool rightTaken;

    public Philosopher(string name, ref Fork left, ref Fork right, Strategy strategy, uint maxStepsToEat, uint minStepsToEat, uint maxStepsToThink, uint minStepsToThink)
    {
        this.state = State.HUNGRY;
        this.name = name;
        this.strategy = strategy;
        this.step = 0;
        this.random = new Random();
        this.maxStepsToEat = maxStepsToEat;
        this.minStepsToEat = minStepsToEat;
        this.maxStepsToThink = maxStepsToThink;
        this.minStepsToThink = minStepsToThink;
        this.stepsToEat = (uint)random.Next((int)minStepsToEat, (int)maxStepsToEat + 1);
        this.stepsToThink = (uint)random.Next((int)minStepsToThink, (int)maxStepsToThink + 1);
        this.stepsToTakeFork = 2;
        this.eaten = 0;
        this.left = left;
        this.right = right;
    }

    public Philosopher(string name, ref Fork left, ref Fork right, Coordinator coordinator, uint maxStepsToEat, uint minStepsToEat, uint maxStepsToThink, uint minStepsToThink)
    {
        this.state = State.HUNGRY;
        this.name = name;
        this.strategy = null;
        this.step = 0;
        this.random = new Random();
        this.maxStepsToEat = maxStepsToEat;
        this.minStepsToEat = minStepsToEat;
        this.maxStepsToThink = maxStepsToThink;
        this.minStepsToThink = minStepsToThink;
        this.stepsToEat = (uint)random.Next((int)minStepsToEat, (int)maxStepsToEat + 1);
        this.stepsToThink = (uint)random.Next((int)minStepsToThink, (int)maxStepsToThink + 1);
        this.stepsToTakeFork = 2;
        this.eaten = 0;
        this.left = left;
        this.right = right;
        this.coordinator = coordinator;
        this.coordinator.Action += Do;
        this.rightTaken = false;
        this.leftTaken = false;
    }

    private void Do(Philosopher philosopher, string action)
    {
        if (philosopher != this)
        {
            return;
        }

        if (action == "Take left") // is it true that only one event will be raised per one step of simulation? if yes then step should be incremented without and if (otherwise no clue how to count steps internally)
        {
            step++;
        }
        switch (state)
        {
            case State.HUNGRY:
                FinishedHungry(action);
                break;

            case State.TAKING_FORK:
                if (step >= stepsToTakeFork)
                {
                    FinishedTakingFork();
                    return;
                }
                break;

            case State.EATING:
                if (step >= stepsToEat)
                {
                    FinishedEating(action);
                    return;
                }
                break;

            case State.THINKING:
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

    private void FinishedHungry(string action)
    {
        switch (action)
        {
            case "Take left":
                TryTakeLeft();
                break;

            case "Take right":
                TryTakeRight();
                break;

            default:
                // Console.Error.WriteLine("ERROR: unknown action \"" + action + "\" from coordinator to " + name);
                break;
        }
        if (leftTaken && rightTaken)
        {
            step = 0;
            state = State.TAKING_FORK;
        }
        else
        {
            step = 0;
            state = State.HUNGRY;
            // ReleaseForks();
        }
    }

    private void FinishedEating(string action)
    {
        // switch (action)
        // {
        //     case "Release left":
        //         left.Release();
        //         break;

        //     case "Release right":
        //         right.Release();
        //         break;

        //     default:
        //         // Console.Error.WriteLine("ERROR: unknown action \"" + action + "\" from coordinator to " + name);
        //         break;
        // }
        ReleaseForks();
        if (!leftTaken && !rightTaken)
        {
            eaten++;
            step = 0;
            stepsToEat = (uint)random.Next((int)minStepsToEat, (int)maxStepsToEat + 1);
            state = State.THINKING;
        }
        else
        {
            step = 0;
            // ReleaseForks();
        }
    }

    private void TryTakeLeft()
    {
        try
        {
            left.Take(name);
            leftTaken = true;
        }
        catch (System.Exception e)
        {
            // Console.Error.WriteLine(name + ": failed to take the left fork: " + e.Message);
        }
    }

    private void TryTakeRight()
    {
        try
        {
            right.Take(name);
            rightTaken = true;
        }
        catch (System.Exception e)
        {
            // Console.Error.WriteLine(name + ": failed to take the right fork: " + e.Message);
        }
    }

    public void MakeMove()
    {
        step++;
        switch (state)
        {
            case State.HUNGRY:
                FinishedHungry();
                break;

            case State.TAKING_FORK:
                if (step >= stepsToTakeFork)
                {
                    FinishedTakingFork();
                    return;
                }
                break;

            case State.EATING:
                if (step >= stepsToEat)
                {
                    FinishedEating();
                    return;
                }
                break;

            case State.THINKING:
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

    private void FinishedThinking()
    {
        step = 0;
        stepsToThink = (uint)random.Next((int)minStepsToThink, (int)maxStepsToThink + 1);
        state = State.HUNGRY;
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
                    state = State.HUNGRY;
                    step = 0;
                    left.Release();
                    // Console.Error.WriteLine(name + ": failed to take the right fork: " + e.Message);
                    return;
                }
            }
            catch (System.Exception e)
            {
                state = State.HUNGRY;
                step = 0;
                // Console.Error.WriteLine(name + ": failed to take the left fork: " + e.Message);
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
                    state = State.HUNGRY;
                    step = 0;
                    right.Release();
                    Console.Error.WriteLine(name + ": failed to take the left fork: " + e.Message);
                    return;
                }
            }
            catch (System.Exception e)
            {
                state = State.HUNGRY;
                step = 0;
                Console.Error.WriteLine(name + ": failed to take the right fork: " + e.Message);
                return;
            }
        }
        step = 0;
        state = State.TAKING_FORK;
    }

    private void FinishedEating()
    {
        eaten++;
        step = 0;
        stepsToEat = (uint)random.Next((int)minStepsToEat, (int)maxStepsToEat + 1);
        ReleaseForks();
        state = State.THINKING;
    }

    private void FinishedTakingFork()
    {
        step = 0;
        state = State.EATING;
    }



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
        switch (state)
        {
            case State.HUNGRY:
                return "Available actions: TakeRightFork | TakeLeftFork";

            case State.TAKING_FORK:
                return Convert.ToString(stepsToTakeFork - step) + " steps left";

            case State.EATING:
                return Convert.ToString(stepsToEat - step) + " steps left";

            case State.THINKING:
                return Convert.ToString(stepsToThink - step) + " steps left";

            default:
                return "";
        }
    }

    // public uint GetEaten()
    // {
    //     return eaten;
    // }
}