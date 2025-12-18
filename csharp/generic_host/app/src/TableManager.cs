namespace app;

public interface ITableManager
{
    (ForkState Left, ForkState Right) GetForksFor(int philosopherIndex);
    IReadOnlyList<ForkState> Forks { get; }
}

public class TableManager : ITableManager
{
    private readonly ForkState[] forks;

    public TableManager(SimulationOptions options)
    {
        forks = Enumerable.Range(1, options.Philosophers.Length)
            .Select(id => new ForkState(id))
            .ToArray();
    }

    public IReadOnlyList<ForkState> Forks => forks;

    public (ForkState Left, ForkState Right) GetForksFor(int philosopherIndex)
    {
        ForkState left = forks[philosopherIndex];
        ForkState right = forks[(philosopherIndex + 1) % forks.Length];
        return (left, right);
    }
}

