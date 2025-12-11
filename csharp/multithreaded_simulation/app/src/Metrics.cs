using System.Diagnostics;
using strategy;

class Metrics
{
    private readonly Philosopher[] philosophers;
    private readonly Fork[] forks;
    private readonly Stopwatch stopwatch;

    public Metrics(Fork[] forks, Philosopher[] philosophers)
    {
        this.forks = forks;
        this.philosophers = philosophers;
        stopwatch = Stopwatch.StartNew();
    }

    public void Print()
    {
        TimeSpan elapsed = stopwatch.Elapsed;
        Console.WriteLine("\n=================== METRICS ===================");
        Console.WriteLine("Elapsed: " + elapsed.TotalMilliseconds.ToString("F0") + " ms");

        // Score (total eaten)
        int eatenTotal = 0;
        foreach (var p in philosophers)
        {
            eatenTotal += p.GetEaten();
        }
        Console.WriteLine("Score (total eaten): " + eatenTotal);
        Console.WriteLine();

        // Throughput
        Console.WriteLine("Throughput (meals per ms):");
        double elapsedMs = Math.Max(elapsed.TotalMilliseconds, 1.0);
        double throughputSum = 0;
        foreach (var p in philosophers)
        {
            double t = p.GetEaten() / elapsedMs;
            throughputSum += t;
            Console.WriteLine("  " + p.GetName() + ": " + t.ToString("F4"));
        }
        double avgThroughput = philosophers.Length > 0 ? throughputSum / philosophers.Length : 0;
        Console.WriteLine("  Average: " + avgThroughput.ToString("F4"));
        Console.WriteLine();

        // Waiting time
        Console.WriteLine("Waiting time (ms spent hungry):");
        double sumWait = 0;
        double maxWait = 0;
        string maxWaitPhilosopher = "";
        foreach (var p in philosophers)
        {
            double wait = p.GetTotalHungry().TotalMilliseconds;
            sumWait += wait;
            if (wait > maxWait)
            {
                maxWait = wait;
                maxWaitPhilosopher = p.GetName();
            }
            Console.WriteLine("  " + p.GetName() + ": " + wait.ToString("F1") + " ms");
        }
        double avgWait = philosophers.Length > 0 ? sumWait / philosophers.Length : 0;
        Console.WriteLine("  Average: " + avgWait.ToString("F1") + " ms");
        Console.WriteLine("  Maximum: " + maxWait.ToString("F1") + " ms (" + maxWaitPhilosopher + ")");
        Console.WriteLine();

        // Fork utilization
        Console.WriteLine("Fork utilization (%):");
        foreach (var fork in forks)
        {
            (double availablePct, double inUsePct) = fork.GetUtilization(elapsed);
            Console.WriteLine("  Fork-" + fork.Id + ": Available " + availablePct.ToString("F1") + "% | InUse " + inUsePct.ToString("F1") + "%");
        }
    }
}