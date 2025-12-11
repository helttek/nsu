using System;
using System.Collections.Generic;

namespace strategy
{
    public static class LockTracker
    {
        private static readonly object sync = new();
        private static readonly Dictionary<string, HashSet<int>> held = new(StringComparer.Ordinal);
        private static readonly Dictionary<string, HashSet<int>> waiting = new(StringComparer.Ordinal);

        public static void RegisterAcquired(string philosopher, int forkId)
        {
            lock (sync)
            {
                if (!held.TryGetValue(philosopher, out var s)) { s = new HashSet<int>(); held[philosopher] = s; }
                s.Add(forkId);
                if (waiting.TryGetValue(philosopher, out var w)) w.Remove(forkId);
            }
        }

        public static void RegisterReleased(string philosopher, int forkId)
        {
            lock (sync)
            {
                if (held.TryGetValue(philosopher, out var s)) s.Remove(forkId);
            }
        }

        public static void RegisterWaiting(string philosopher, int forkId)
        {
            lock (sync)
            {
                if (!waiting.TryGetValue(philosopher, out var s)) { s = new HashSet<int>(); waiting[philosopher] = s; }
                s.Add(forkId);
            }
        }

        public static void ClearWaiting(string philosopher, int forkId)
        {
            lock (sync)
            {
                if (waiting.TryGetValue(philosopher, out var s)) s.Remove(forkId);
            }
        }

        public static Dictionary<string, List<string>> BuildWaitForGraph(Func<int, string?> forkOwnerResolver)
        {
            var graph = new Dictionary<string, List<string>>(StringComparer.Ordinal);
            lock (sync)
            {
                foreach (var kv in waiting)
                {
                    var p = kv.Key;
                    if (!graph.ContainsKey(p)) graph[p] = new List<string>();
                    foreach (var forkId in kv.Value)
                    {
                        var owner = forkOwnerResolver(forkId);
                        if (!string.IsNullOrEmpty(owner) && owner != p)
                        {
                            graph[p].Add(owner!);
                        }
                    }
                }
            }
            return graph;
        }
    }
}
