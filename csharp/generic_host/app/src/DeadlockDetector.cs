using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using strategy;

namespace app
{
    public class DeadlockDetector : IDisposable
    {
        private readonly Dictionary<string, IPhilosopher> _philosophers;
        private readonly Timer _timer;

        public event Action<List<string>>? OnDeadlockDetected;

        public DeadlockDetector(IEnumerable<IPhilosopher> philosophers, TimeSpan pollInterval)
        {
            _philosophers = philosophers.ToDictionary(p => p.GetName());
            _timer = new Timer(_ => Check(), null, pollInterval, pollInterval);
        }

        private void Check()
        {
            foreach (var p in _philosophers.Values)
            {
                if (p.GetState() != PhilosopherStage.Hungry) continue;

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

                    if (current.GetState() != PhilosopherStage.Hungry)
                    {
                        break;
                    }

                    string action = current.GetAction();
                    IFork wanted = null;
                    if (action == "TakeLeftFork")
                        wanted = current.Left;
                    else if (action == "TakeRightFork")
                        wanted = current.Right;
                    if (wanted == null) break;
                    string ownerName = wanted.UsedBy();

                    if (string.IsNullOrEmpty(ownerName))
                    {
                        break;
                    }

                    if (ownerName == current.GetName())
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
