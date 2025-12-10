namespace strategy;

public class AlwaysRightStrategy : Strategy
{
    public AlwaysRightStrategy()
    {
    }

    public override string NextMove()
    {
        return "right";
    }
}