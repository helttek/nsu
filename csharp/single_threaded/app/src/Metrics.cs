using coordinator;
using strategy;

class Metrics
{
    private Philosopher[] philosophers;
    private Fork[] forks;
    private double[] eaten;

    public Metrics(Fork[] forks, Philosopher[] philosophers)
    {
        this.forks = forks;
        this.philosophers = philosophers;
        eaten = new double[philosophers.Length];
    }

    public void GetData()
    {
        for (int i = 0; i < philosophers.Length; i++)
        {
            eaten[i] = philosophers[i].GetEaten();
        }
        // Print aggregated metrics snapshot
        Print();
    }

    public void Print()
    {
        Console.WriteLine("=================Metrics=================");

        Console.WriteLine("Throughput:");
        uint eatenTotal = 0;
        for (int i = 0; i < philosophers.Length; i++)
        {
            uint e = philosophers[i].GetEaten();
            eatenTotal += e;
            Console.WriteLine(philosophers[i].GetName() + ": " + e);
        }
        Console.WriteLine("Total eaten (Score): " + eatenTotal);

        // Waiting time metrics
        Console.WriteLine();
        Console.WriteLine("Waiting time (hungry state):");
        uint sumWait = 0;
        uint maxWait = 0;
        string maxWaitPhilosopher = "";
        for (int i = 0; i < philosophers.Length; i++)
        {
            uint w = philosophers[i].GetTotalHungry();
            sumWait += w;
            if (w > maxWait)
            {
                maxWait = w;
                maxWaitPhilosopher = philosophers[i].GetName();
            }
            Console.WriteLine(philosophers[i].GetName() + ": waited " + w + " steps");
        }
        double avgWait = philosophers.Length > 0 ? (double)sumWait / philosophers.Length : 0;
        Console.WriteLine("Average waiting time: " + avgWait + " steps; Max: " + maxWait + " (" + maxWaitPhilosopher + ")");

        // Fork utilization snapshot (instantaneous)
        Console.WriteLine();
        Console.WriteLine("Forks utilization (instant snapshot):");
        int inUse = 0;
        for (int i = 0; i < forks.Length; i++)
        {
            var state = forks[i].GetState();
            Console.Write("Fork-" + (i + 1) + ": " + state);
            if (state == "IN_USE" || state == "IN_USE")
            {
                Console.WriteLine(" (used by: " + forks[i].UsedBy() + ")");
                inUse++;
            }
            else
            {
                Console.WriteLine();
            }
        }
        double utilization = forks.Length > 0 ? (double)inUse / forks.Length * 100.0 : 0.0;
        Console.WriteLine("Forks in use: " + inUse + "/" + forks.Length + " (" + utilization + "% )");
    }
}