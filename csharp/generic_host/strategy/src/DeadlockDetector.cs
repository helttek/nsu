using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace strategy
{
    public class DeadlockDetector : IDisposable
    {
        private readonly IReadOnlyList<Fork> forks;
        private readonly Timer timer;

        public event Action<List<string>>? OnDeadlockDetected;

        public DeadlockDetector(IReadOnlyList<Fork> forks, TimeSpan pollInterval)
        {
            this.forks = forks;
            timer = new Timer(_ => Check(), null, pollInterval, pollInterval);
        }

        private void Check()
        {
            string? Resolver(int forkId)
            {
                var fork = forks.FirstOrDefault(f => f.Id == forkId);
                var owner = fork?.UsedBy();
                if (string.IsNullOrEmpty(owner)) return null;
                return owner;
            }

            var graph = LockTracker.BuildWaitForGraph(Resolver);
            var cycle = FindCycle(graph);
            if (cycle != null && cycle.Count > 0)
            {
                OnDeadlockDetected?.Invoke(cycle);
            }
        }

        private static List<string>? FindCycle(Dictionary<string, List<string>> graph)
        {
            var visited = new HashSet<string>(StringComparer.Ordinal);
            var stack = new HashSet<string>(StringComparer.Ordinal);
            var parent = new Dictionary<string, string>(StringComparer.Ordinal);

            foreach (var node in graph.Keys)
            {
                if (Dfs(node)) return ExtractCycle(node);
            }

            return null;

            bool Dfs(string v)
            {
                if (stack.Contains(v)) return true;
                if (visited.Contains(v)) return false;
                visited.Add(v);
                stack.Add(v);
                if (graph.TryGetValue(v, out var edges))
                {
                    foreach (var w in edges)
                    {
                        if (!parent.ContainsKey(w)) parent[w] = v;
                        if (Dfs(w)) return true;
                    }
                }
                stack.Remove(v);
                return false;
            }

            List<string>? ExtractCycle(string start)
            {
                var cycle = new List<string>();
                var cur = start;
                var seen = new HashSet<string>(StringComparer.Ordinal);
                while (!seen.Contains(cur))
                {
                    seen.Add(cur);
                    cycle.Add(cur);
                    if (!parent.TryGetValue(cur, out cur)) break;
                }
                cycle.Reverse();
                return cycle;
            }

        }

        public void Dispose() => timer.Dispose();
    }
}
