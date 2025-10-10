using coordinator;
using strategy;

class Metrics
{
    private Philosopher[] philosophers;
    private Fork[] forks;
    private double[] eaten;

    public Metrics(Fork[] forks, Philosopher[] philosophers)
    {
        this.forks = forks;
        this.philosophers = philosophers;
        eaten = new double[philosophers.Length];
    }

    public void GetData()
    {
        for (int i = 0; i < philosophers.Length; i++)
        {
            eaten[i] = philosophers[i].GetEaten();
        }
    }

    public void Print()
    {
        Console.WriteLine("=================Metrics=================");

        Console.WriteLine("Throughput:");
        uint eatenTotal = 0;
        double average = 0;
        for (int i = 0; i < philosophers.Length; i++)
        {
            eatenTotal += philosophers[i].GetEaten();
            // average
            Console.WriteLine(philosophers[i].GetName() + ": " + philosophers[i].GetEaten());
        }
        // Console.WriteLine("Average: " +);
    }
}