using System.Threading;

namespace strategy;

public class AlwaysRightStrategy : Strategy
{
    public override void AcquireForks(Philosopher p, CancellationToken token)
    {
        p.SetAction($"Waiting for Right ({p.Right.Id})");
        p.Right.Acquire(p.GetName(), p.ForkAcquireMs, token);
        p.SetAction($"Holding Right ({p.Right.Id}), Waiting for Left ({p.Left.Id})");
        p.Left.Acquire(p.GetName(), p.ForkAcquireMs, token);
    }

    public override bool TakesLeftFirst(int philosopherIndex)
    {
        return false;
    }
}