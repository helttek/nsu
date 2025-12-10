namespace app;

class Settings
{
    public string[]? Philosophers { get; set; }
    public bool? UseCoordinator { get; set; }
    public string? Strategy { get; set; }
    public uint? MAX_NUMBER_OF_STEPS { get; set; }
    public uint? METRIC_FREQ { get; set; }
    public uint? MAX_STEPS_TO_EAT { get; set; }
    public uint? MIN_STEPS_TO_EAT { get; set; }
    public uint? MAX_STEPS_TO_THINK { get; set; }
    public uint? MIN_STEPS_TO_THINK { get; set; }

    private const uint DEFAULT_MAX_NUM_STEPS = 100;
    private const uint DEFAULT_METRIC_FREQ = 100;
    private const uint DEFAULT_MAX_STEPS_TO_EAT = 5;
    private const uint DEFAULT_MIN_STEPS_TO_EAT = 4;
    private const uint DEFAULT_MAX_STEPS_TO_THINK = 10;
    private const uint DEFAULT_MIN_STEPS_TO_THINK = 3;

    public void Validate()
    {
        if (Philosophers == null)
        {
            throw new Exception("Philosophers names in settings are null");
        }
        if (MAX_NUMBER_OF_STEPS == null)
        {
            Console.Error.WriteLine("WARNING: number of steps in settings is null. Using default value: " + DEFAULT_MAX_NUM_STEPS.ToString() + ".");
            MAX_NUMBER_OF_STEPS = DEFAULT_MAX_NUM_STEPS;
        }
        if (METRIC_FREQ == null)
        {
            Console.Error.WriteLine("WARNING: metric frequency rate is null. Using default value: " + DEFAULT_METRIC_FREQ.ToString() + ".");
            METRIC_FREQ = DEFAULT_METRIC_FREQ;
        }
        if (MAX_STEPS_TO_EAT == null)
        {
            Console.Error.WriteLine("WARNING: maximum steps to eat variable is null. Using default value: " + DEFAULT_MAX_STEPS_TO_EAT.ToString() + ".");
            MAX_STEPS_TO_EAT = DEFAULT_MAX_STEPS_TO_EAT;
        }
        if (MIN_STEPS_TO_EAT == null)
        {
            Console.Error.WriteLine("WARNING: minimum steps to eat variable is null. Using default value: " + DEFAULT_MIN_STEPS_TO_EAT.ToString() + ".");
            MIN_STEPS_TO_EAT = DEFAULT_MIN_STEPS_TO_EAT;
        }
        if (MAX_STEPS_TO_THINK == null)
        {
            Console.Error.WriteLine("WARNING: maximum steps to think variable is null. Using default value: " + DEFAULT_MAX_STEPS_TO_THINK.ToString() + ".");
            MAX_STEPS_TO_THINK = DEFAULT_MAX_STEPS_TO_THINK;
        }
        if (MIN_STEPS_TO_THINK == null)
        {
            Console.Error.WriteLine("WARNING: minimum steps to think variable is null. Using default value: " + DEFAULT_MIN_STEPS_TO_THINK.ToString() + ".");
            MIN_STEPS_TO_THINK = DEFAULT_MIN_STEPS_TO_THINK;
        }
    }
}