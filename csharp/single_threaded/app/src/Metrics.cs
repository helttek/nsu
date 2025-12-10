using coordinator;
using strategy;

class Metrics
{
    private Philosopher[] philosophers;
    private Fork[] forks;
    private uint totalSteps = 0;
    private uint[] forkAvailableSteps;
    private uint[] forkInUseSteps;
    private Philosopher.State[] lastPhilosopherState;
    private uint[] hungerStartStep;
    private uint[] totalHungerSteps;

    public Metrics(Fork[] forks, Philosopher[] philosophers)
    {
        this.forks = forks;
        this.philosophers = philosophers;
        forkAvailableSteps = new uint[forks.Length];
        forkInUseSteps = new uint[forks.Length];
        lastPhilosopherState = new Philosopher.State[philosophers.Length];
        hungerStartStep = new uint[philosophers.Length];
        totalHungerSteps = new uint[philosophers.Length];
        
        // Initialize last state to current state
        for (int i = 0; i < philosophers.Length; i++)
        {
            lastPhilosopherState[i] = philosophers[i].GetState();
        }
    }

    public void RecordStep()
    {
        // Track fork states each step
        for (int i = 0; i < forks.Length; i++)
        {
            if (forks[i].GetState() == "AVAILABLE")
                forkAvailableSteps[i]++;
            else if (forks[i].GetState() == "IN_USE")
                forkInUseSteps[i]++;
        }

        // Track philosopher hunger state transitions
        for (int i = 0; i < philosophers.Length; i++)
        {
            var currentState = philosophers[i].GetState();
            
            // If leaving HUNGRY state, accumulate the hunger time
            if (lastPhilosopherState[i] == Philosopher.State.HUNGRY && 
                currentState != Philosopher.State.HUNGRY)
            {
                totalHungerSteps[i] += hungerStartStep[i];
                hungerStartStep[i] = 0;
            }
            
            // If entering HUNGRY state, start tracking hunger duration
            if (lastPhilosopherState[i] != Philosopher.State.HUNGRY && 
                currentState == Philosopher.State.HUNGRY)
            {
                hungerStartStep[i] = 0;
            }
            
            // If in HUNGRY state, increment hunger duration
            if (currentState == Philosopher.State.HUNGRY)
            {
                hungerStartStep[i]++;
            }
            
            lastPhilosopherState[i] = currentState;
        }

        totalSteps++;
    }

    public void GetData()
    {
        // Placeholder for snapshot metrics if needed
        Print();
    }

    public void Print()
    {
        Console.WriteLine("\n=================== METRICS ===================");

        // Score (total eaten)
        uint eatenTotal = 0;
        for (int i = 0; i < philosophers.Length; i++)
        {
            eatenTotal += philosophers[i].GetEaten();
        }
        Console.WriteLine("Score (total eaten): " + eatenTotal);
        Console.WriteLine();

        // Throughput per 1000 steps
        Console.WriteLine("Throughput (eaten per 1000 steps):");
        double throughputScale = totalSteps > 0 ? 1000.0 / totalSteps : 1.0;
        double sumThroughput = 0;
        for (int i = 0; i < philosophers.Length; i++)
        {
            uint e = philosophers[i].GetEaten();
            double throughput = e * throughputScale;
            sumThroughput += throughput;
            Console.WriteLine("  " + philosophers[i].GetName() + ": " + throughput.ToString("F2"));
        }
        double avgThroughput = philosophers.Length > 0 ? sumThroughput / philosophers.Length : 0;
        Console.WriteLine("  Average: " + avgThroughput.ToString("F2"));
        Console.WriteLine();

        // Waiting time metrics (tracked via state observation)
        Console.WriteLine("Waiting time (steps spent hungry):");
        uint sumWait = 0;
        uint maxWait = 0;
        string maxWaitPhilosopher = "";
        for (int i = 0; i < philosophers.Length; i++)
        {
            uint w = totalHungerSteps[i];
            sumWait += w;
            if (w > maxWait)
            {
                maxWait = w;
                maxWaitPhilosopher = philosophers[i].GetName();
            }
            Console.WriteLine("  " + philosophers[i].GetName() + ": " + w + " steps");
        }
        double avgWait = philosophers.Length > 0 ? (double)sumWait / philosophers.Length : 0;
        Console.WriteLine("  Average: " + avgWait.ToString("F2") + " steps");
        Console.WriteLine("  Maximum: " + maxWait + " steps (" + maxWaitPhilosopher + ")");
        Console.WriteLine();

        // Fork utilization
        Console.WriteLine("Fork utilization (% of time in each state):");
        for (int i = 0; i < forks.Length; i++)
        {
            uint available = forkAvailableSteps[i];
            uint inUse = forkInUseSteps[i];
            uint total = available + inUse;
            
            double pctAvailable = total > 0 ? (double)available / total * 100.0 : 0;
            double pctInUse = total > 0 ? (double)inUse / total * 100.0 : 0;
            
            Console.WriteLine("  Fork-" + (i + 1) + ": Available " + pctAvailable.ToString("F1") + "% | InUse " + pctInUse.ToString("F1") + "%");
        }
    }
}