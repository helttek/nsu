using System.Text.Json;
using coordinator;
using strategy;
namespace app;

class Program
{
    static void Main()
    {
        Settings? settings = JsonSerializer.Deserialize<Settings>(File.ReadAllText("appsettings.json"));
        ValidateSettings(settings);

        Philosopher[] philosophers = new Philosopher[settings.Philosophers.Length];
        Fork[] forks = new Fork[settings.Philosophers.Length];
        if (settings.UseCoordinator == true)
        {
            for (int i = 0; i < philosophers.Length; i++)
            {
                if (i == philosophers.Length - 1)
                {
                    philosophers[i] = new Philosopher(settings.Philosophers[i]);
                }
                philosophers[i] = new Philosopher(settings.Philosophers[i]);
            }
            Run(philosophers, new Coordinator(philosophers));
        }
        else
        {
            Strategy strategy = GetStrategy(settings.Strategy);
            for (int i = 0; i < philosophers.Length; i++)
            {
                philosophers[i] = new Philosopher(settings.Philosophers[i], strategy);
            }
            Run(philosophers);
        }
    }

    private static void ValidateSettings(Settings? settings)
    {
        if (settings == null)
        {
            throw new Exception("settings are null");
        }
        if (settings.Philosophers == null)
        {
            throw new Exception("Philosophers names in settings are null");
        }
    }

    private static Strategy GetStrategy(string? strategy)
    {
        if (strategy != null)
        {
            switch (strategy)
            {
                case "AlwaysRight":
                    return new AlwaysRight();

                default:
                    Console.WriteLine("No known strategy was provided, using default one.");
                    return new AlwaysRight();
            }
        }
        return new AlwaysRight();
    }

    private static void Run(Philosopher[] philosophers)
    {
        uint step = 0;
        const uint MAX_NUM_STEPS = 100;
        while (step != MAX_NUM_STEPS)
        {
            for (int i = 0; i < philosophers.Length; i++)
            {
                philosophers[i].MakeMove();
            }
            PrintState(philosophers, step);
            step++;
        }
    }

    private static void PrintState(Philosopher[] philosophers, uint currentStep)
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

    }

    //TODO: 
    // - where should the forks be? in the Philosopher class?
    // - keep track of forks (create them as an array and pass them as an argument to a Philosoper constructor)
    // - add forks to a constructor in Philosopher class

    private static void Run(Philosopher[] philosophers, Coordinator coordinator) { }
}