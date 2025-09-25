using System.Text.Json;
using coordinator;
using strategy;
namespace app;

class Program
{
    static void Main()
    {
        string settingsFileName = "appsettings.json";
        string settingsFileString = File.ReadAllText(settingsFileName);
        Settings? settings = JsonSerializer.Deserialize<Settings>(settingsFileString);
        if (settings == null)
        {
            Console.WriteLine("settings are null");
            return;
        }
        if (settings.Philosophers == null)
        {
            Console.WriteLine("Philosophers names in settings are null");
            return;
        }

        Philosopher[] philosophers = new Philosopher[settings.Philosophers.Length];
        Coordinator coordinator;
        if (settings.UseCoordinator == true)
        {
            for (int i = 0; i < philosophers.Length; i++)
            {
                philosophers[i] = new Philosopher(settings.Philosophers[i]);
            }
            coordinator = new(philosophers);
            Run(philosophers, coordinator);
            return;
        }
        Strategy defaultStrategy = new AlwaysRight();
        Strategy strategy;
        if (settings.Strategy != null)
        {
            switch (settings.Strategy)
            {
                case "AlwaysRight":
                    strategy = new AlwaysRight();
                    break;

                default:
                    Console.WriteLine("No known strategy was provided, using default one.");
                    strategy = defaultStrategy;
                    break;
            }
        }
        else
        {
            strategy = defaultStrategy;
        }
        for (int i = 0; i < philosophers.Length; i++)
        {
            philosophers[i] = new Philosopher(settings.Philosophers[i], strategy);
        }
        Run(philosophers);
    }

    private static void Run(Philosopher[] philosophers)
    {
        uint step = 0;
        const uint MAX_NUM_STEPS = 1000000;
        while (step != MAX_NUM_STEPS)
        {
            for (int i = 0; i < philosophers.Length; i++)
            {
                
            }
            step++;
        }
    }

    private static void Run(Philosopher[] philosophers, Coordinator coordinator) { }
}