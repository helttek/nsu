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
        // Notify all philosophers a tick occurred so they can advance their internal timers
        for (int i = 0; i < philosophers.Length; i++)
        {
            Action?.Invoke(philosophers[i], "Tick");
        }

        bool anyAllocation = false;
        // Try to allocate forks to hungry philosophers when both adjacent forks are available
        for (int i = 0; i < forks.Length; i++)
        {
            var p = philosophers[i];
            if (p.GetState() == Philosopher.State.HUNGRY)
            {
                int leftIdx = i;
                int rightIdx = (i + 1) % forks.Length;
                if (forks[leftIdx].IsAvailable() && forks[rightIdx].IsAvailable())
                {
                    // coordinator takes forks on behalf of philosopher
                    try
                    {
                        forks[leftIdx].Take(p.GetName());
                        forks[rightIdx].Take(p.GetName());
                        // inform philosopher that forks were granted
                        Action?.Invoke(p, "GrantLeft");
                        Action?.Invoke(p, "GrantRight");
                        anyAllocation = true;
                    }
                    catch (System.Exception)
                    {
                        // if take failed for some reason, ensure consistency
                    }
                }
            }
        }

        // Deadlock detection: if no allocation happened this step and all philosophers are hungry
        bool allHungry = true;
        for (int i = 0; i < philosophers.Length; i++)
        {
            if (philosophers[i].GetState() != Philosopher.State.HUNGRY)
            {
                allHungry = false;
                break;
            }
        }
        if (!anyAllocation && allHungry)
        {
            Console.Error.WriteLine("DEADLOCK detected by Coordinator at step " + stepCounter + ". All philosophers are hungry and no forks could be allocated.");
        }

        stepCounter++;
    }
}
