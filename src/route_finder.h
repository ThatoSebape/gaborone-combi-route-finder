#pragma once

#include "combi_data.h"
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

// One leg of a journey: get from one hub to the next via a specific route.
struct PathStep {
    std::string fromHub;
    std::string toHub;
    std::string viaRoute;
};

// Breadth-first search over the adjacency list. BFS guarantees the result
// uses the fewest hops (transfers) — the right notion of "shortest" here,
// since we have no real distance/time data to weigh routes against.
//
// Returns the sequence of steps from start to destination. An empty vector
// means no path exists between them. (Caller should handle start == destination
// separately, before calling this — that's not a "no path" situation.)
inline std::vector<PathStep> findRoute(
    const std::string& start,
    const std::string& destination,
    const std::unordered_map<std::string, std::vector<Connection>>& adjacency)
{
    // hub -> {the hub we reached it from, the route we used to get there}
    std::unordered_map<std::string, std::pair<std::string, std::string>> cameFrom;
    std::unordered_set<std::string> visited;
    std::queue<std::string> toVisit;

    visited.insert(start);
    toVisit.push(start);

    bool found = false;
    while (!toVisit.empty() && !found) {
        std::string current = toVisit.front();
        toVisit.pop();

        auto it = adjacency.find(current);
        if (it == adjacency.end()) continue;

        for (const auto& conn : it->second) {
            if (visited.count(conn.toHub)) continue;

            visited.insert(conn.toHub);
            cameFrom[conn.toHub] = {current, conn.viaRoute};

            if (conn.toHub == destination) {
                found = true;
                break;
            }
            toVisit.push(conn.toHub);
        }
    }

    if (!found) return {};

    // Walk backward from destination to start using cameFrom, then reverse
    // so the result reads start -> ... -> destination.
    std::vector<PathStep> path;
    std::string node = destination;
    while (node != start) {
        const auto& [prevHub, route] = cameFrom[node];
        path.push_back(PathStep{prevHub, node, route});
        node = prevHub;
    }
    std::reverse(path.begin(), path.end());
    return path;
}