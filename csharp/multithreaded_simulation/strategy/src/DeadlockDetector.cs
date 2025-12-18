using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace strategy
{
    public class DeadlockDetector : IDisposable
    {
        private readonly Dictionary<string, Philosopher> _philosophers;
        private readonly Timer _timer;

        public event Action<List<string>>? OnDeadlockDetected;

        public DeadlockDetector(IEnumerable<Philosopher> philosophers, TimeSpan pollInterval)
        {
            _philosophers = philosophers.ToDictionary(p => p.GetName());
            _timer = new Timer(_ => Check(), null, pollInterval, pollInterval);
        }

        private void Check()
        {
            foreach (var p in _philosophers.Values)
            {
                if (p.GetState() != Philosopher.State.HUNGRY) continue;

                var visited = new HashSet<string>();
                var current = p;
                var path = new List<string>();

                while (current != null)
                {
                    if (visited.Contains(current.GetName()))
                    {
                        path.Add(current.GetName());
                        OnDeadlockDetected?.Invoke(path);
                        return;
                    }

                    visited.Add(current.GetName());
                    path.Add(current.GetName());

                    if (current.GetState() != Philosopher.State.HUNGRY)
                    {
                        break;
                    }

                    bool leftFirst = current.Strategy.TakeWhichFork(current.Index);
                    var first = leftFirst ? current.Left : current.Right;
                    var second = leftFirst ? current.Right : current.Left;

                    Fork wantedFork;

                    if (first.UsedBy() == current.GetName())
                    {
                        wantedFork = second;
                    }
                    else
                    {
                        wantedFork = first;
                    }

                    string ownerName = wantedFork.UsedBy();

                    if (string.IsNullOrEmpty(ownerName))
                    {
                        break;
                    }

                    if (!_philosophers.TryGetValue(ownerName, out var owner))
                    {
                        break;
                    }

                    current = owner;
                }
            }
        }

        public void Dispose() => _timer.Dispose();
    }
}