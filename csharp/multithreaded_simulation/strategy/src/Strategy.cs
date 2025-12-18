namespace strategy;

public abstract class Strategy
{
    protected Strategy() { }

    // Returns true if the philosopher should attempt the left fork first.
    public abstract bool TakeWhichFork(int philosopherIndex);
}
