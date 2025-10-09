namespace strategy;

public class Fork
{
    public enum State : byte
    {
        Available,
        InUse
    }

    private State state;
    private string? usedBy;

    public Fork()
    {
        this.state = State.Available;
        usedBy = null;
    }

    public void Take(string philosopherName)
    {
        if (state == State.Available)
        {
            state = State.InUse; // should there be a state "IsBeingTaken"
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
        state = State.Available;
        usedBy = null;
    }

    public bool IsAvailable()
    {
        return state == State.Available;
    }
}