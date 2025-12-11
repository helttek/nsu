using System.Collections.Concurrent;

namespace app;

public interface IPhilosopherRegistry
{
    PhilosopherState Register(string name, int index);
    IReadOnlyList<PhilosopherState> GetAll();
}

public sealed class PhilosopherRegistry : IPhilosopherRegistry
{
    private readonly ConcurrentDictionary<string, PhilosopherState> states = new(StringComparer.Ordinal);

    public PhilosopherState Register(string name, int index)
    {
        return states.GetOrAdd(name, _ => new PhilosopherState(name, index));
    }

    public IReadOnlyList<PhilosopherState> GetAll()
    {
        return states.Values.OrderBy(s => s.Index).ToArray();
    }
}

