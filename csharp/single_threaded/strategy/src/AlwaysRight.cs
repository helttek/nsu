namespace strategy;

public class AlwaysRight : Strategy
{
    public AlwaysRight()
    {
    }

    public override string NextMove()
    {
        return "right";
    }
}