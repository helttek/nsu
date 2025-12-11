using app;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using strategy;

namespace App;

internal class Program
{
    private static async Task Main(string[] args)
    {
        var builder = Host.CreateApplicationBuilder(args);

        // Take an early snapshot to know how many philosophers to register.
        SimulationOptions simOptions = new();
        builder.Configuration.GetSection("Simulation").Bind(simOptions);
        simOptions.EnsureDefaults();

        builder.Services.Configure<SimulationOptions>(builder.Configuration.GetSection("Simulation"));
        builder.Services.AddSingleton(simOptions);

        builder.Services.AddSingleton<ITableManager>(sp => new TableManager(simOptions));
        builder.Services.AddSingleton<IPhilosopherRegistry, PhilosopherRegistry>();
        builder.Services.AddSingleton<IMetricsCollector, MetricsCollector>();

        builder.Services.AddSingleton<Strategy>(sp =>
        {
            string? name = simOptions.Strategy;
            return name switch
            {
                "AlwaysRight" => new AlwaysRightStrategy(),
                "LeftRight" => new LeftRightStrategy(),
                _ => new LeftRightStrategy()
            };
        });

        // Register each philosopher as a separate hosted service.
        for (int i = 0; i < simOptions.Philosophers.Length; i++)
        {
            int index = i;
            string philosopherName = simOptions.Philosophers[i];
            Console.WriteLine($"Registering philosopher hosted service: {philosopherName}");
            builder.Services.AddSingleton<IHostedService>(sp => new PhilosopherHostedService(
                philosopherName,
                index,
                sp.GetRequiredService<ITableManager>(),
                sp.GetRequiredService<Strategy>(),
                sp.GetRequiredService<IPhilosopherRegistry>(),
                sp.GetRequiredService<IOptions<SimulationOptions>>(),
                sp.GetRequiredService<ILogger<PhilosopherHostedService>>()));
        }

        builder.Services.AddHostedService<StatusPrinterHostedService>();

        using IHost host = builder.Build();
        await host.RunAsync();
    }
}
