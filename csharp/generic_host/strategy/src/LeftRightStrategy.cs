namespace strategy;

/// <summary>
/// Alternates fork pickup order to reduce contention:
/// even-indexed philosophers take left first, odd take right first.
/// </summary>
public sealed class LeftRightStrategy : IPhilosopherStrategy
{
    public bool TakeLeftFirst(int philosopherIndex)
    {
        return philosopherIndex % 2 == 0;
    }
}

