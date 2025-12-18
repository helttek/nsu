using System.Diagnostics;
using Microsoft.Extensions.Logging;

namespace app;

public interface IMetricsCollector
{
    IReadOnlyList<PhilosopherSnapshot> GetPhilosophers();
    IReadOnlyList<ForkSnapshot> GetForks();
    void Print(TimeSpan elapsed);
}

public class MetricsCollector : IMetricsCollector
{
    private readonly IPhilosopherRegistry registry;
    private readonly ITableManager tableManager;
    private readonly ILogger<MetricsCollector> logger;

    public MetricsCollector(IPhilosopherRegistry registry, ITableManager tableManager, ILogger<MetricsCollector> logger)
    {
        this.registry = registry;
        this.tableManager = tableManager;
        this.logger = logger;
    }

    public IReadOnlyList<PhilosopherSnapshot> GetPhilosophers()
    {
        return registry.GetAll()
            .Select(p => p.Snapshot())
            .ToArray();
    }

    public IReadOnlyList<ForkSnapshot> GetForks()
    {
        return tableManager.Forks.Select(f => f.GetSnapshot()).ToArray();
    }

    public void Print(TimeSpan elapsed)
    {
        var philosophers = GetPhilosophers();
        var forks = GetForks();

        logger.LogInformation("===== METRICS =====");
        logger.LogInformation("Elapsed: {Elapsed} ms", elapsed.TotalMilliseconds.ToString("F0"));

        double elapsedMs = Math.Max(elapsed.TotalMilliseconds, 1);
        double totalEaten = philosophers.Sum(p => p.Meals);
        logger.LogInformation("Score (total eaten): {Total}", totalEaten);

        logger.LogInformation("Throughput (meals/ms):");
        foreach (var p in philosophers)
        {
            double t = p.Meals / elapsedMs;
            logger.LogInformation("  {Name}: {Throughput:F4}", p.Name, t);
        }

        logger.LogInformation("Waiting time (ms hungry):");
        double maxWait = 0;
        string maxWaitName = string.Empty;
        double sumWait = 0;
        foreach (var p in philosophers)
        {
            double wait = p.HungryFor.TotalMilliseconds;
            sumWait += wait;
            if (wait > maxWait)
            {
                maxWait = wait;
                maxWaitName = p.Name;
            }
            logger.LogInformation("  {Name}: {Wait:F1}", p.Name, wait);
        }

        double avgWait = philosophers.Count > 0 ? sumWait / philosophers.Count : 0;
        logger.LogInformation("  Average: {Avg:F1}", avgWait);
        logger.LogInformation("  Maximum: {Max:F1} ({Name})", maxWait, maxWaitName);

        logger.LogInformation("Fork utilization (%):");
        foreach (var fork in forks)
        {
            TimeSpan total = fork.Available + fork.InUse;
            double totalMs = Math.Max(total.TotalMilliseconds, 1);
            double availablePct = fork.Available.TotalMilliseconds / totalMs * 100;
            double inUsePct = fork.InUse.TotalMilliseconds / totalMs * 100;
            logger.LogInformation("  Fork-{Id}: Available {Available:F1}% | InUse {InUse:F1}%", fork.Id, availablePct, inUsePct);
        }
    }
}

