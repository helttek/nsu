namespace strategy;

public class AlwaysRight : Strategy
{
    private Fork right;
    private Fork left;

    public AlwaysRight()
    {
        this.left = new();
        this.right = new();
    }

    public override void MakeMove()
    {
        try
        {
            right.Take();
        }
        catch (System.Exception)
        {
            throw;
        }
        try
        {
            left.Take();
        }
        catch (System.Exception)
        {
            throw;
        }
    }
}