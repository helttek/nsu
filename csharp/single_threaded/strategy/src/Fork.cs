namespace strategy;

public class Fork
{
    private enum State : byte
    {
        Available,
        InUse
    }

    private State state;

    public Fork()
    {
        this.state = State.Available;
    }

    public void Take() { }
}