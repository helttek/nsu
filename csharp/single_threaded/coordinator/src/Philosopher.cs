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
    private uint totalHungrySteps;
    private string name;
    private State state;
    private Strategy? strategy;
    private readonly Fork right;
    private readonly Fork left;
    private Coordinator? coordinator;
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
        this.leftTaken = false;
        this.rightTaken = false;
        this.totalHungrySteps = 0;
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
        this.totalHungrySteps = 0;
    }

    // Called by Coordinator via event in coordinator-driven simulation
    private void Do(Philosopher philosopher, string action)
    {
        if (philosopher != this)
        {
            return;
        }

        if (action == "Tick")
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

    // Coordinator-driven: react to grants
    private void FinishedHungry(string action)
    {
        switch (action)
        {
            case "GrantLeft":
                leftTaken = true;
                break;
            case "GrantRight":
                rightTaken = true;
                break;
            case "Tick":
                // no-op here; step already incremented
                break;
            default:
                // unknown action
                break;
        }

        if (leftTaken && rightTaken)
        {
            // accumulate waiting time spent hungry
            totalHungrySteps += step;
            step = 0;
            state = State.TAKING_FORK;
        }
    }

    private void FinishedEating()
    {
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
        }
    }

    private void TryTakeLeft()
    {
        try
        {
            left.Take(name);
            leftTaken = true;
        }
        catch (System.Exception)
        {
            // ignore
        }
    }

    private void TryTakeRight()
    {
        try
        {
            right.Take(name);
            rightTaken = true;
        }
        catch (System.Exception)
        {
            // ignore
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
        leftTaken = false;
        rightTaken = false;
    }

    private void FinishedThinking()
    {
        step = 0;
        stepsToThink = (uint)random.Next((int)minStepsToThink, (int)maxStepsToThink + 1);
        state = State.HUNGRY;
    }

    // Strategy-driven hungry handling (no coordinator)
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
                catch (System.Exception)
                {
                    state = State.HUNGRY;
                    step = 0;
                    left.Release();
                    return;
                }
            }
            catch (System.Exception)
            {
                state = State.HUNGRY;
                step = 0;
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
                catch (System.Exception)
                {
                    state = State.HUNGRY;
                    step = 0;
                    right.Release();
                    return;
                }
            }
            catch (System.Exception)
            {
                state = State.HUNGRY;
                step = 0;
                return;
            }
        }
        // when using a strategy (no coordinator), the philosopher actually takes forks here
        step = 0;
        state = State.TAKING_FORK;
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

    public uint GetTotalHungry()
    {
        return totalHungrySteps;
    }

    // Public command entry point - philosopher only performs actions it's told.
    public void OnCommand(string command)
    {
        switch (command)
        {
            case "TakeLeft":
                TryTakeLeft();
                break;
            case "TakeRight":
                TryTakeRight();
                break;
            case "GrantLeft":
                leftTaken = true;
                break;
            case "GrantRight":
                rightTaken = true;
                break;
            case "Release":
                ReleaseForks();
                break;
            case "Tick":
                Do(this, "Tick");
                break;
            default:
                // unknown command - ignore
                break;
        }

        // If both forks are taken (either by TryTake or by Grant), transition to TAKING_FORK
        if (leftTaken && rightTaken && state == State.HUNGRY)
        {
            state = State.TAKING_FORK;
            step = 0;
        }
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
}