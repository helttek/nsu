using strategy;

namespace coordinator;

public class Coordinator
{
    public event Action<Philosopher, string>? Action;
    private Philosopher[] philosophers;
    private Fork[] forks;
    private Random random;

    public Coordinator(Philosopher[] philosophers, Fork[] forks)
    {
        this.philosophers = philosophers;
        this.forks = forks;
        this.random = new();
    }

    public void SimulateStep()
    {
        for (int i = 0; i < forks.Length; i++)
        {
            if (forks[i].IsAvailable() && forks[(i + 1) % forks.Length].IsAvailable())
            {
                Action?.Invoke(philosophers[i], "Take right");
                Action?.Invoke(philosophers[i], "Take left");
            }
        }
    }
}
