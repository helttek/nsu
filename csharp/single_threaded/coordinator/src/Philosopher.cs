namespace coordinator;

using strategy;

public class Philosopher
{
    private enum State : byte
    {
        Thinking,
        Hungry,
        Eating
    }

    private string name { get; }

    private State state;

    private Strategy? strategy;

    public Philosopher(string name, Strategy strategy)
    {
        this.state = State.Hungry;
        this.name = name;
        this.strategy = strategy;
    }

    public Philosopher(string name)
    {
        this.state = State.Hungry;
        this.name = name;
        this.strategy = null;
    }

    public void MakeMove()
    {
        strategy?.MakeMove();
    }


}