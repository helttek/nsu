using System.Text.Json;
using coordinator;
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
        for (int i = 0; i < philosophers.Length; i++)
        {
            philosophers[i] = new Philosopher(settings.Philosophers[i]);
        }

        Coordinator coordinator = new(philosophers);

        int step = 0;
        const int MAX_NUM_STEPS = 1000000;
        while (step != MAX_NUM_STEPS)
        {
            for (int i = 0; i < settings?.Philosophers?.Length; i++)
            {

            }
            step++;
        }
    }
}