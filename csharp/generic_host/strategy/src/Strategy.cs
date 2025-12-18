namespace strategy;

public interface IPhilosopherStrategy
{
    // Returns true if the philosopher should attempt the left fork first.
    bool TakeLeftFirst(int philosopherIndex);
}
