namespace strategy;

public class AlwaysRightStrategy : IPhilosopherStrategy
{
    public AlwaysRightStrategy()
    {
    }

    public bool TakeLeftFirst(int philosopherIndex)
    {
        // Always take the right fork first (left goes second).
        return false;
    }
}