using strategy;

namespace app;

public class PhilosopherState
{
    private readonly object sync = new();
    private DateTime? hungrySince;
    private TimeSpan hungryTotal = TimeSpan.Zero;

    public PhilosopherState(string name, int index)
    {
        Name = name;
        Index = index;
    }

    public string Name { get; }
    public int Index { get; }

    public strategy.PhilosopherStage Stage { get; private set; } = strategy.PhilosopherStage.Thinking;
    public string Action { get; private set; } = "Thinking";
    public int Meals { get; private set; }

    public void SetThinking(int msLeft)
    {
        lock (sync)
        {
            Stage = strategy.PhilosopherStage.Thinking;
            Action = msLeft > 0 ? $"Thinking ({msLeft} ms left)" : "Thinking";
        }
    }

    public void SetHungry(string action)
    {
        lock (sync)
        {
            if (Stage != strategy.PhilosopherStage.Hungry)
            {
                hungrySince = DateTime.UtcNow;
            }
            Stage = strategy.PhilosopherStage.Hungry;
            Action = action;
        }
    }

    public void SetAction(string action)
    {
        lock (sync)
        {
            Action = action;
        }
    }

    public void SetEating(int msLeft)
    {
        lock (sync)
        {
            if (hungrySince.HasValue)
            {
                hungryTotal += DateTime.UtcNow - hungrySince.Value;
                hungrySince = null;
            }

            Stage = strategy.PhilosopherStage.Eating;
            Action = $"Eating ({msLeft} ms left)";
        }
    }

    public void IncrementMeals()
    {
        lock (sync)
        {
            Meals++;
        }
    }

    public PhilosopherSnapshot Snapshot()
    {
        lock (sync)
        {
            TimeSpan totalHungry = hungryTotal;
            if (Stage == strategy.PhilosopherStage.Hungry && hungrySince.HasValue)
            {
                totalHungry += DateTime.UtcNow - hungrySince.Value;
            }

            return new PhilosopherSnapshot(Name, Stage, Action, Meals, totalHungry);
        }
    }
}

public readonly record struct PhilosopherSnapshot(
    string Name,
    PhilosopherStage Stage,
    string Action,
    int Meals,
    TimeSpan HungryFor);

