namespace strategy;

public class Fork
{
    public enum State : byte
    {
        AVAILABLE,
        IN_USE
    }

    private State state;
    private string? usedBy;

    public Fork()
    {
        this.state = State.AVAILABLE;
        usedBy = null;
    }

    public void Take(string philosopherName)
    {
        if (state == State.AVAILABLE)
        {
            state = State.IN_USE;
            usedBy = philosopherName;
        }
        else
        {
            throw new Exception("Fork already in use.");
        }
    }

    public string UsedBy()
    {
        return usedBy ?? "";
    }

    public string GetState()
    {
        return state.ToString();
    }

    public void Release()
    {
        state = State.AVAILABLE;
        usedBy = null;
    }

    public bool IsAvailable()
    {
        return state == State.AVAILABLE;
    }
}