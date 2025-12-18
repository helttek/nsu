namespace app;

public class SimulationOptions
{
    public string[] Philosophers { get; set; } = Array.Empty<string>();
    public string? Strategy { get; set; }
    public int SimDurationMs { get; set; } = DEFAULT_SIM_DURATION_MS;
    public int StatusIntervalMs { get; set; } = DEFAULT_STATUS_INTERVAL_MS;
    public int ForkAcquireMs { get; set; } = DEFAULT_FORK_ACQUIRE_MS;
    public int ThinkMinMs { get; set; } = DEFAULT_THINK_MIN_MS;
    public int ThinkMaxMs { get; set; } = DEFAULT_THINK_MAX_MS;
    public int EatMinMs { get; set; } = DEFAULT_EAT_MIN_MS;
    public int EatMaxMs { get; set; } = DEFAULT_EAT_MAX_MS;

    public int? DeadlockDetectionIntervalMs { get; set; } = DEFAULT_DEADLOCK_DETECTION_INTERVAL_MS;

    public int DurationSeconds => SimDurationMs / 1000;

    private const int DEFAULT_SIM_DURATION_MS = 10_000;
    private const int DEFAULT_STATUS_INTERVAL_MS = 150;
    private const int DEFAULT_FORK_ACQUIRE_MS = 20;
    private const int DEFAULT_THINK_MIN_MS = 30;
    private const int DEFAULT_THINK_MAX_MS = 100;
    private const int DEFAULT_EAT_MIN_MS = 40;
    private const int DEFAULT_EAT_MAX_MS = 50;
    private const int DEFAULT_DEADLOCK_DETECTION_INTERVAL_MS = 100;

    public void Validate()
    {
        if (Philosophers is null || Philosophers.Length == 0)
        {
            Philosophers = ["Plato", "Aristotle", "Socrates", "Descartes", "Kant"];
        }

        if (ThinkMaxMs < ThinkMinMs)
        {
            ThinkMaxMs = ThinkMinMs;
        }

        if (EatMaxMs < EatMinMs)
        {
            EatMaxMs = EatMinMs;
        }

        if (DeadlockDetectionIntervalMs == null)
        {
            DeadlockDetectionIntervalMs = DEFAULT_DEADLOCK_DETECTION_INTERVAL_MS;
        }
    }
}