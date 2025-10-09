using System.Text.Json;
using coordinator;
using strategy;
namespace app;

class Program
{
    private const uint DEFAULT_MAX_NUM_STEPS = 100;
    static void Main()
    {
        Settings? settings = GetSettings("appsettings.json");
        ValidateSettings(ref settings);

        Philosopher[] philosophers = new Philosopher[settings.Philosophers.Length];
        Fork[] forks = CreateForks(settings.Philosophers.Length);
        if (settings.UseCoordinator == true)
        {
            Coordinator coordinator = new(philosophers, forks);
            for (int i = 0; i < philosophers.Length; i++)
            {
                philosophers[i] = new Philosopher(settings.Philosophers[i], ref forks[i], ref forks[(i + 1) % philosophers.Length], coordinator);
            }
            Run(ref philosophers, ref coordinator, forks, settings.MaxNumberOfSteps);
        }
        else
        {
            Strategy strategy = GetStrategy(settings.Strategy);
            for (int i = 0; i < philosophers.Length; i++)
            {
                philosophers[i] = new Philosopher(settings.Philosophers[i], ref forks[i], ref forks[(i + 1) % philosophers.Length], strategy);
            }
            Run(philosophers, forks, settings.MaxNumberOfSteps);
        }
    }

    private static Settings GetSettings(string fileName)
    {
        try
        {
            var settingsString = File.ReadAllText(fileName);
            try
            {
                return JsonSerializer.Deserialize<Settings>(settingsString);
            }
            catch (System.Exception e)
            {
                throw new Exception("Failed to load settings: " + e.Message);
            }
        }
        catch (System.Exception e)
        {
            throw new Exception("Failed to read file as text: " + e.Message);
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

    private static void ValidateSettings(ref Settings? settings)
    {
        if (settings == null)
        {
            throw new Exception("settings are null");
        }
        if (settings.Philosophers == null)
        {
            throw new Exception("Philosophers names in settings are null");
        }
        if (settings.MaxNumberOfSteps == null)
        {
            Console.Error.WriteLine("ERROR: number of steps in settings is null. Using defaulr value: " + DEFAULT_MAX_NUM_STEPS.ToString() + ".");
            settings.MaxNumberOfSteps = DEFAULT_MAX_NUM_STEPS;
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

    private static void Run(Philosopher[] philosophers, Fork[] forks, uint? MAX_NUM_STEPS)
    {
        uint step = 0;
        while (step != MAX_NUM_STEPS)
        {
            PrintState(philosophers, step, forks);
            for (int i = 0; i < philosophers.Length; i++)
            {
                philosophers[i].MakeMove();
            }
            step++;
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

    private static void Run(ref Philosopher[] philosophers, ref Coordinator coordinator, Fork[] forks, uint? MAX_NUM_STEPS)
    {
        uint step = 0;
        while (step != MAX_NUM_STEPS)
        {
            PrintState(philosophers, step, forks);
            coordinator.SimulateStep();
            step++;
        }
    }
}