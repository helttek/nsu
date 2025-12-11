using System.Diagnostics;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace app;

public sealed class StatusPrinterHostedService : BackgroundService
{
    private readonly IPhilosopherRegistry registry;
    private readonly ITableManager tableManager;
    private readonly IMetricsCollector metrics;
    private readonly IHostApplicationLifetime lifetime;
    private readonly SimulationOptions options;
    private readonly ILogger<StatusPrinterHostedService> logger;
    private readonly Stopwatch stopwatch = Stopwatch.StartNew();

    public StatusPrinterHostedService(
        IPhilosopherRegistry registry,
        ITableManager tableManager,
        IMetricsCollector metrics,
        IOptions<SimulationOptions> options,
        IHostApplicationLifetime lifetime,
        ILogger<StatusPrinterHostedService> logger)
    {
        this.registry = registry;
        this.tableManager = tableManager;
        this.metrics = metrics;
        this.lifetime = lifetime;
        this.options = options.Value;
        this.logger = logger;
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        uint step = 0;

        while (!stoppingToken.IsCancellationRequested && stopwatch.ElapsedMilliseconds < options.SimDurationMs)
        {
            PrintState(step);
            step++;
            try
            {
                await Task.Delay(options.StatusIntervalMs, stoppingToken).ConfigureAwait(false);
            }
            catch (OperationCanceledException)
            {
                break;
            }
        }

        lifetime.StopApplication();
    }

    public override Task StopAsync(CancellationToken cancellationToken)
    {
        PrintState(null);
        metrics.Print(stopwatch.Elapsed);
        return base.StopAsync(cancellationToken);
    }

    private void PrintState(uint? step)
    {
        var philosophers = registry.GetAll().Select(p => p.Snapshot()).ToArray();
        var forks = tableManager.Forks.Select(f => f.GetSnapshot()).ToArray();

        Console.WriteLine();
        Console.WriteLine("===== STEP " + (step.HasValue ? (step.Value + 1).ToString() : "FINAL") + " =====");
        Console.WriteLine("Philosophers:");
        foreach (var p in philosophers)
        {
            Console.WriteLine($"  {p.Name}: {p.Stage} (Action = {p.Action}), eaten: {p.Meals}");
        }

        Console.WriteLine();
        Console.WriteLine("Forks:");
        foreach (var f in forks)
        {
            string state = f.Status == ForkStatus.InUse
                ? $"InUse (used by {f.UsedBy})"
                : "Available";
            Console.WriteLine($"  Fork-{f.Id}: {state}");
        }
    }
}

