using System.Net.Http.Metrics;
using System.Text.Json;
using coordinator;
using Microsoft.Extensions.Hosting;
using strategy;
using Microsoft.Extensions.Configuration;

namespace app;

class Program
{
    static void Main()
    {
        Settings? settings = Host.CreateApplicationBuilder().Configuration.Get<Settings>();
        settings!.Validate();

        Philosopher[] philosophers = new Philosopher[settings.Philosophers!.Length];
        Fork[] forks = CreateForks(settings.Philosophers.Length);
        if (settings.UseCoordinator == true)
        {
            Coordinator coordinator = new(philosophers, forks);
            for (int i = 0; i < philosophers.Length; i++)
            {
                philosophers[i] = new Philosopher(settings.Philosophers[i], ref forks[i], ref forks[(i + 1) % philosophers.Length], coordinator, settings.MAX_STEPS_TO_EAT!.Value, settings.MIN_STEPS_TO_EAT!.Value, settings.MAX_STEPS_TO_THINK!.Value, settings.MIN_STEPS_TO_THINK!.Value);
            }
            Run(settings, ref philosophers, ref coordinator, forks, settings.MAX_NUMBER_OF_STEPS);
        }
        else
        {
            Strategy strategy = GetStrategy(settings.Strategy);
            for (int i = 0; i < philosophers.Length; i++)
            {
                philosophers[i] = new Philosopher(settings.Philosophers[i], ref forks[i], ref forks[(i + 1) % philosophers.Length], strategy, settings.MAX_STEPS_TO_EAT!.Value, settings.MIN_STEPS_TO_EAT!.Value, settings.MAX_STEPS_TO_THINK!.Value, settings.MIN_STEPS_TO_THINK!.Value);
            }
            Run(settings, philosophers, forks, settings.MAX_NUMBER_OF_STEPS, strategy);
        }
    }

    private static Fork[] CreateForks(int numForks)
    {
        Fork[] forks = new Fork[numForks];
        for (int i = 0; i < numForks; ++i)
        {
            forks[i] = new();
        }
        return forks;
    }

    private static Strategy GetStrategy(string? strategy)
    {
        if (strategy != null)
        {
            switch (strategy)
            {
                case "AlwaysRight":
                    return new AlwaysRightStrategy();

                default:
                    Console.WriteLine("WARNING: No known strategy was provided, using default one: Always Right.");
                    return new AlwaysRightStrategy();
            }
        }
        return new AlwaysRightStrategy();
    }

    private static void Run(Settings settings, Philosopher[] philosophers, Fork[] forks, uint? MAX_NUM_STEPS, Strategy strategy)
    {
        Metrics metrics = new(forks, philosophers);
        uint step = 0;
        while (step != MAX_NUM_STEPS)
        {
            PrintState(philosophers, step, forks);
            for (int i = 0; i < philosophers.Length; i++)
            {
                // Strategy tells the philosopher which fork to try taking when hungry.
                if (philosophers[i].GetState() == coordinator.Philosopher.State.HUNGRY)
                {
                    string move = strategy.NextMove();
                    if (move == "left") philosophers[i].OnCommand("TakeLeft");
                    else philosophers[i].OnCommand("TakeRight");
                }

                // Advance internal philosopher timers / states
                philosophers[i].MakeMove();
            }
            step++;
            if (step == settings.METRIC_FREQ - 1)
            {
                metrics.GetData();
            }// metrics.Print();
        }
    }

    private static void PrintState(Philosopher[] philosophers, uint currentStep, Fork[] forks)
    {
        Console.WriteLine("=================== STEP " + Convert.ToString(currentStep + 1) + " ===================");
        Console.WriteLine("Philosophers:");
        for (int i = 0; i < philosophers.Length; i++)
        {
            Console.Write("  " + philosophers[i].GetName() + ": ");
            Console.Write(philosophers[i].GetState());
            Console.Write(" (" + philosophers[i].GetCurrentAction() + "); ");
            Console.WriteLine("eaten: " + Convert.ToString(philosophers[i].GetEaten()));
        }
        Console.WriteLine();
        Console.WriteLine("Forks:");
        for (int i = 0; i < forks.Length; i++)
        {
            Console.Write("Fork-" + Convert.ToString(i + 1) + ": " + forks[i].GetState());
            if (forks[i].GetState() == "InUse")
            {
                Console.WriteLine(" (" + forks[i].UsedBy() + ")");
            }
            else
            {
                Console.WriteLine();
            }
        }
        Console.WriteLine();
    }

    private static void Run(Settings settings, ref Philosopher[] philosophers, ref Coordinator coordinator, Fork[] forks, uint? MAX_NUM_STEPS)
    {
        Metrics metrics = new(forks, philosophers);
        uint step = 0;
        while (step != MAX_NUM_STEPS)
        {
            PrintState(philosophers, step, forks);
            coordinator.SimulateStep();
            step++;
        }
        if (step == settings.METRIC_FREQ - 1)
        {
            metrics.GetData();
        }
    }

    //TODO: 
    // - clean up enums and hardcoded values
    // - the logic of choosing which and how forks to take should be in the strategy class
    // - fix the coordinator events for better reusability
}