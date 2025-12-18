using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using strategy;

namespace app;

public sealed class PhilosopherHostedService : BackgroundService, IPhilosopher
{
    private readonly string name;
    private readonly int index;
    private readonly ForkState left;
    private readonly ForkState right;
    private readonly IPhilosopherStrategy strategy;
    private readonly PhilosopherState state;
    private readonly SimulationOptions options;
    private readonly ILogger<PhilosopherHostedService> logger;
    private readonly Random random;

    public PhilosopherHostedService(
        string name,
        int index,
        ITableManager tableManager,
        IPhilosopherStrategy strategy,
        IPhilosopherRegistry registry,
        IOptions<SimulationOptions> options,
        ILogger<PhilosopherHostedService> logger)
    {
        this.name = name;
        this.index = index;
        (left, right) = tableManager.GetForksFor(index);
        this.strategy = strategy;
        state = registry.Register(name, index);
        this.options = options.Value;
        this.logger = logger;
        random = new Random(Environment.TickCount + index);
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        logger.LogInformation("Philosopher service started: {Name}", name);
        while (!stoppingToken.IsCancellationRequested)
        {
            await ThinkAsync(stoppingToken).ConfigureAwait(false);
            if (stoppingToken.IsCancellationRequested) break;
            await HungryAsync(stoppingToken).ConfigureAwait(false);
        }
    }

    private async Task ThinkAsync(CancellationToken token)
    {
        int duration = random.Next(options.ThinkMinMs, options.ThinkMaxMs + 1);
        state.SetThinking(duration);
        await DelaySafe(duration, token).ConfigureAwait(false);
        state.SetHungry("TakeLeftFork|TakeRightFork");
    }

    private async Task HungryAsync(CancellationToken token)
    {
        while (!token.IsCancellationRequested)
        {
            bool leftFirst = strategy.TakeLeftFirst(index);
            var first = leftFirst ? left : right;
            var second = leftFirst ? right : left;

            bool firstHeld = false;
            bool secondHeld = false;

            try
            {
                if (!await TryTakeFork(first, leftFirst ? "TakeLeftFork" : "TakeRightFork", token).ConfigureAwait(false))
                {
                    await Backoff(token).ConfigureAwait(false);
                    continue;
                }

                firstHeld = true;

                if (!await TryTakeFork(second, leftFirst ? "TakeRightFork" : "TakeLeftFork", token).ConfigureAwait(false))
                {
                    await Backoff(token).ConfigureAwait(false);
                    continue;
                }

                secondHeld = true;
                int eatDuration = random.Next(options.EatMinMs, options.EatMaxMs + 1);
                state.SetEating(eatDuration);
                await DelaySafe(eatDuration, token).ConfigureAwait(false);
                state.IncrementMeals();
                state.SetThinking(0);
                return;
            }
            finally
            {
                if (secondHeld)
                {
                    second.Release(name);
                    state.SetAction("ReleaseFork");
                }

                if (firstHeld)
                {
                    first.Release(name);
                    state.SetAction("ReleaseFork");
                }
            }
        }
    }

    private async Task<bool> TryTakeFork(ForkState fork, string action, CancellationToken token)
    {
        state.SetAction(action);
        bool acquired = await fork.TryTakeAsync(name, options.ForkAcquireMs, token).ConfigureAwait(false);
        if (!acquired && token.IsCancellationRequested)
        {
            logger.LogDebug("{Name} cancelled while taking fork {Fork}", name, fork.Id);
        }
        return acquired;
    }

    private async Task Backoff(CancellationToken token)
    {
        int pause = random.Next(5, 15);
        state.SetAction($"Backoff ({pause} ms)");
        await DelaySafe(pause, token).ConfigureAwait(false);
    }

    private static async Task DelaySafe(int ms, CancellationToken token)
    {
        try
        {
            await Task.Delay(ms, token).ConfigureAwait(false);
        }
        catch (OperationCanceledException)
        {
            // ignore, cooperative cancellation
        }
    }

    public string GetName() => name;
    public int Index => index;
    public strategy.PhilosopherStage GetState() => state.Stage;
    public IPhilosopherStrategy Strategy => strategy;
    public IFork Left => left;
    public IFork Right => right;
    public string GetAction() => state.Action;
}

