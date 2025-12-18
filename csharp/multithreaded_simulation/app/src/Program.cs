using System.Diagnostics;
using System.Threading;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Hosting;
using strategy;

namespace app;

class Program
{
    static void Main()
    {
        Settings? settings = Host.CreateApplicationBuilder().Configuration.Get<Settings>();
        settings!.Validate();

        string[] philosophersNames = settings.Philosophers!;
        Fork[] forks = CreateForks(philosophersNames.Length);
        Strategy strategy = GetStrategy(settings.Strategy);

        using CancellationTokenSource cts = new();
        Philosopher[] philosophers = CreatePhilosophers(philosophersNames, forks, strategy, cts.Token, settings);

        using DeadlockDetector detector = new DeadlockDetector(philosophers, TimeSpan.FromMilliseconds(settings.DeadlockDetectionIntervalMs!.Value));
        detector.OnDeadlockDetected += cycle =>
        {
            Console.WriteLine("DEADLOCK DETECTED: " + string.Join(" -> ", cycle));
            cts.Cancel();
        };

        foreach (var p in philosophers)
        {
            p.Start();
        }

        Run(philosophers, forks, cts, settings);
    }

    private static Philosopher[] CreatePhilosophers(string[] names, Fork[] forks, Strategy strategy, CancellationToken token, Settings settings)
    {
        Philosopher[] philosophers = new Philosopher[names.Length];
        for (int i = 0; i < names.Length; i++)
        {
            Fork left = forks[i];
            Fork right = forks[(i + 1) % names.Length];
            philosophers[i] = new Philosopher(
                names[i],
                i,
                left,
                right,
                strategy,
                token,
                settings.ForkAcquireMs!.Value,
                settings.ThinkMinMs!.Value,
                settings.ThinkMaxMs!.Value,
                settings.EatMinMs!.Value,
                settings.EatMaxMs!.Value);
        }
        return philosophers;
    }

    private static Fork[] CreateForks(int numForks)
    {
        Fork[] forks = new Fork[numForks];
        for (int i = 0; i < numForks; ++i)
        {
            forks[i] = new Fork(i + 1);
        }
        return forks;
    }

    private static Strategy GetStrategy(string? strategyName)
    {
        if (strategyName != null)
        {
            switch (strategyName)
            {
                case "AlwaysRight":
                    return new AlwaysRightStrategy();
                default:
                    Console.WriteLine("WARNING: No known strategy was provided, using default one: AlwaysRight.");
                    return new AlwaysRightStrategy();
            }
        }
        return new AlwaysRightStrategy();
    }

    private static void Run(Philosopher[] philosophers, Fork[] forks, CancellationTokenSource cts, Settings settings)
    {
        Metrics metrics = new(forks, philosophers);
        Stopwatch stopwatch = Stopwatch.StartNew();
        uint step = 0;

        while (stopwatch.ElapsedMilliseconds < settings.SimDurationMs!.Value)
        {
            PrintState(philosophers, step, forks);
            Thread.Sleep(settings.StatusIntervalMs!.Value);
            step++;
        }

        cts.Cancel();

        metrics.Print();
    }

    private static void PrintState(Philosopher[] philosophers, uint currentStep, Fork[] forks)
    {
        Console.WriteLine("=================== STEP " + Convert.ToString(currentStep + 1) + " ===================");
        Console.WriteLine("Philosophers:");
        for (int i = 0; i < philosophers.Length; i++)
        {
            Console.Write("  " + philosophers[i].GetName() + ": ");
            Console.Write(philosophers[i].GetState());
            Console.Write(" (Action = " + philosophers[i].GetCurrentAction() + "), eaten: ");
            Console.WriteLine(Convert.ToString(philosophers[i].GetEaten()));
        }
        Console.WriteLine();
        Console.WriteLine("Forks:");
        for (int i = 0; i < forks.Length; i++)
        {
            Console.Write("  Fork-" + Convert.ToString(i + 1) + ": " + forks[i].GetState());
            if (forks[i].GetState() == Fork.State.IN_USE.ToString())
            {
                Console.WriteLine(" (used by " + forks[i].UsedBy() + ")");
            }
            else
            {
                Console.WriteLine();
            }
        }
        Console.WriteLine();
    }
}