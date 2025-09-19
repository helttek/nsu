using strategy;

class AlwaysRight(Fork right, Fork left) : Strategy
{
    private Fork right = right;
    private Fork left = left;

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