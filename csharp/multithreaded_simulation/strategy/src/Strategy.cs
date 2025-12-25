using System.Threading;

namespace strategy;

public abstract class Strategy
{
    public abstract void AcquireForks(Philosopher p, CancellationToken token);

    public virtual void ReleaseForks(Philosopher p)
    {
        p.Left.Release(p.GetName());
        p.Right.Release(p.GetName());
    }

    public abstract bool TakesLeftFirst(int philosopherIndex);
}