using strategy;

namespace coordinator;

public class Coordinator
{
    public event Action<Philosopher, string>? Action;
    private Philosopher[] philosophers;
    private Fork[] forks;
    private Random random;
    private uint stepCounter = 0;

    public Coordinator(Philosopher[] philosophers, Fork[] forks)
    {
        this.philosophers = philosophers;
        this.forks = forks;
        this.random = new();
    }

    public void SimulateStep()
    {
        for (int i = 0; i < philosophers.Length; i++)
        {
            Action?.Invoke(philosophers[i], "Tick");
        }

        bool anyForksTaken = false;
        for (int i = 0; i < forks.Length; i++)
        {
            var p = philosophers[i];
            if (p.GetState() == Philosopher.State.HUNGRY)
            {
                int leftIdx = i;
                int rightIdx = (i + 1) % forks.Length;
                if (forks[leftIdx].IsAvailable() && forks[rightIdx].IsAvailable())
                {
                    Action?.Invoke(p, "TakeLeft");
                    Action?.Invoke(p, "TakeRight");
                    anyForksTaken = true;
                }
            }
        }

        bool allHungry = true;
        for (int i = 0; i < philosophers.Length; i++)
        {
            if (philosophers[i].GetState() != Philosopher.State.HUNGRY)
            {
                allHungry = false;
                break;
            }
        }
        if (!anyForksTaken && allHungry)
        {
            Console.Error.WriteLine("ERROR: deadlock detected by Coordinator at step " + stepCounter + ". All philosophers are hungry and no philosophers could take both forks.");
        }

        stepCounter++;
    }
}
