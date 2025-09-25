namespace coordinator;

public class Coordinator
{
    public Coordinator(Philosopher[] philosophers)
    {
        for (int i = 0; i < philosophers.Length; i++)
        {
            philosophers[i].IsHungry += HandleHungryPhilosopher;
        }
    }

    private void HandleHungryPhilosopher(Philosopher philosopher)
    {
        philosopher.TakeLeftFork();
    }
}
