namespace strategy;

public class AlwaysRightStrategy : Strategy
{
    public AlwaysRightStrategy()
    {
    }

    public override bool TakeWhichFork(int philosopherIndex)
    {
        // Always take the right fork first (left goes second).
        return false;
    }
}