using app;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using strategy;
using System.Timers;

namespace App;

internal class Program
{
    private static async Task Main(string[] args)
    {
        var builder = Host.CreateApplicationBuilder(args);

        SimulationOptions simOptions = new();
        builder.Configuration.GetSection("Simulation").Bind(simOptions);
        simOptions.Validate();

        builder.Services.AddSingleton(Options.Create(simOptions));

        builder.Services.AddSingleton<ITableManager>(sp => new TableManager(simOptions));
        builder.Services.AddSingleton<IMetricsCollector, MetricsCollector>();
        builder.Services.AddSingleton<IPhilosopherRegistry, PhilosopherRegistry>();

        IPhilosopherStrategy GetStrategy(string name) => name switch
        {
            "Plato" => new LeftRightStrategy(),
            _ => new AlwaysRightStrategy()
        };

        for (int i = 0; i < simOptions.Philosophers.Length; i++)
        {
            int index = i;
            string philosopherName = simOptions.Philosophers[i];
            Console.WriteLine($"Registering philosopher hosted service: {philosopherName}");
            builder.Services.AddSingleton<IHostedService>(sp => new PhilosopherHostedService(
                philosopherName,
                index,
                sp.GetRequiredService<ITableManager>(),
                GetStrategy(philosopherName),
                sp.GetRequiredService<IPhilosopherRegistry>(),
                sp.GetRequiredService<IOptions<SimulationOptions>>(),
                sp.GetRequiredService<ILogger<PhilosopherHostedService>>()));
        }

        builder.Services.AddHostedService<StatusPrinterHostedService>();

        builder.Services.AddSingleton<DeadlockDetector>(sp =>
        {
            var philosophers = sp.GetRequiredService<IEnumerable<IHostedService>>().OfType<IPhilosopher>();
            return new DeadlockDetector(philosophers, TimeSpan.FromMilliseconds(simOptions.DeadlockDetectionIntervalMs!.Value));
        });

        using IHost host = builder.Build();

        var lifetime = host.Services.GetRequiredService<IHostApplicationLifetime>();
        var detector = host.Services.GetRequiredService<DeadlockDetector>();
        detector.OnDeadlockDetected += cycle =>
        {
            Console.WriteLine("DEADLOCK DETECTED: " + string.Join(" -> ", cycle));
            lifetime.StopApplication();
        };

        var sysTimer = new System.Timers.Timer(simOptions.DurationSeconds * 1000) { AutoReset = false };
        sysTimer.Elapsed += (s, e) => lifetime.StopApplication();
        sysTimer.Start();

        await host.RunAsync();

        sysTimer.Dispose();
    }
}
