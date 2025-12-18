namespace strategy;

public enum PhilosopherStage
{
    Thinking,
    Hungry,
    Eating
}

public interface IPhilosopher
{
    string GetName();
    int Index { get; }
    PhilosopherStage GetState();
    string GetAction();
    IPhilosopherStrategy Strategy { get; }
    IFork Left { get; }
    IFork Right { get; }
}

public interface IFork
{
    string? UsedBy();
}