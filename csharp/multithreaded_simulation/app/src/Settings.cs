namespace app;

class Settings
{
    public string[]? Philosophers { get; set; }
    public string? Strategy { get; set; }
    public int? SimDurationMs { get; set; }
    public int? StatusIntervalMs { get; set; }
    public int? ForkAcquireMs { get; set; }
    public int? ThinkMinMs { get; set; }
    public int? ThinkMaxMs { get; set; }
    public int? EatMinMs { get; set; }
    public int? EatMaxMs { get; set; }

    private const int DEFAULT_SIM_DURATION_MS = 10000;
    private const int DEFAULT_STATUS_INTERVAL_MS = 150;
    private const int DEFAULT_FORK_ACQUIRE_MS = 20;
    private const int DEFAULT_THINK_MIN_MS = 30;
    private const int DEFAULT_THINK_MAX_MS = 100;
    private const int DEFAULT_EAT_MIN_MS = 40;
    private const int DEFAULT_EAT_MAX_MS = 50;

    public void Validate()
    {
        if (Philosophers == null)
        {
            throw new Exception("Philosophers names in settings are null");
        }

        if (SimDurationMs == null)
        {
            Console.Error.WriteLine("WARNING: SimDurationMs is null. Using default value: " + DEFAULT_SIM_DURATION_MS.ToString() + ".");
            SimDurationMs = DEFAULT_SIM_DURATION_MS;
        }
        if (StatusIntervalMs == null)
        {
            Console.Error.WriteLine("WARNING: StatusIntervalMs is null. Using default value: " + DEFAULT_STATUS_INTERVAL_MS.ToString() + ".");
            StatusIntervalMs = DEFAULT_STATUS_INTERVAL_MS;
        }
        if (ForkAcquireMs == null)
        {
            Console.Error.WriteLine("WARNING: ForkAcquireMs is null. Using default value: " + DEFAULT_FORK_ACQUIRE_MS.ToString() + ".");
            ForkAcquireMs = DEFAULT_FORK_ACQUIRE_MS;
        }
        if (ThinkMinMs == null)
        {
            Console.Error.WriteLine("WARNING: ThinkMinMs is null. Using default value: " + DEFAULT_THINK_MIN_MS.ToString() + ".");
            ThinkMinMs = DEFAULT_THINK_MIN_MS;
        }
        if (ThinkMaxMs == null)
        {
            Console.Error.WriteLine("WARNING: ThinkMaxMs is null. Using default value: " + DEFAULT_THINK_MAX_MS.ToString() + ".");
            ThinkMaxMs = DEFAULT_THINK_MAX_MS;
        }
        if (EatMinMs == null)
        {
            Console.Error.WriteLine("WARNING: EatMinMs is null. Using default value: " + DEFAULT_EAT_MIN_MS.ToString() + ".");
            EatMinMs = DEFAULT_EAT_MIN_MS;
        }
        if (EatMaxMs == null)
        {
            Console.Error.WriteLine("WARNING: EatMaxMs is null. Using default value: " + DEFAULT_EAT_MAX_MS.ToString() + ".");
            EatMaxMs = DEFAULT_EAT_MAX_MS;
        }
    }
}