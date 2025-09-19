namespace coordinator;

using strategy;

public class Coordinator : IObservable<Fork>
{
    private Philosopher[] observers;

    public Coordinator(int philosophersNum)
    {
        this.observers = new Philosopher[philosophersNum];
    }

    public IDisposable Subscribe(IObserver<Fork> observer)
    {
        
    }
}
