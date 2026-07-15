#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>

using namespace std;

struct Edge {
    string to;
    double time;
    double cost;
};

struct Node {
    string name;
    vector<Edge> edges;
};

class Graph {
private:
    unordered_map<string, Node> nodes;

public:
    void addNode(const string& name) {
        if (!nodes.count(name)) nodes[name] = Node{name, {}};
    }

    void addEdge(const string& from, const string& to, double time, double cost) {
        addNode(from);
        addNode(to);
        nodes[from].edges.push_back({to, time, cost});
    }

    bool loadFromCSV(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return false;

        string line;
        getline(file, line); // header

        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string from, to, timeStr, costStr;

            getline(ss, from, ',');
            getline(ss, to, ',');
            getline(ss, timeStr, ',');
            getline(ss, costStr, ',');

            addEdge(from, to, stod(timeStr), stod(costStr));
        }
        return true;
    }

    bool contains(const string& city) const {
        return nodes.count(city);
    }

    int size() const {
        return static_cast<int>(nodes.size());
    }

    pair<double, vector<string>> shortestPath(
        const string& source,
        const string& destination,
        bool useTime
    ) const {
        const double INF = numeric_limits<double>::infinity();

        unordered_map<string, double> dist;
        unordered_map<string, string> parent;

        for (const auto& [name, node] : nodes) {
            dist[name] = INF;
        }

        using State = pair<double, string>;
        priority_queue<State, vector<State>, greater<State>> pq;

        dist[source] = 0.0;
        pq.push({0.0, source});

        while (!pq.empty()) {
            auto [currentDist, current] = pq.top();
            pq.pop();

            if (currentDist != dist[current]) continue;
            if (current == destination) break;

            for (const Edge& edge : nodes.at(current).edges) {
                double weight = useTime ? edge.time : edge.cost;
                double newDist = currentDist + weight;

                if (newDist < dist[edge.to]) {
                    dist[edge.to] = newDist;
                    parent[edge.to] = current;
                    pq.push({newDist, edge.to});
                }
            }
        }

        if (dist[destination] == INF) {
            return {INF, {}};
        }

        vector<string> path;
        string current = destination;

        while (current != source) {
            path.push_back(current);
            current = parent[current];
        }

        path.push_back(source);
        reverse(path.begin(), path.end());

        return {dist[destination], path};
    }
};

int main() {
    Graph graph;

    if (!graph.loadFromCSV("cities.csv")) {
        cerr << "Error: could not open cities.csv\n";
        return 1;
    }

    cout << "Smart City Route Planner\n";
    cout << "Loaded " << graph.size() << " city nodes.\n\n";

    string source, destination;
    int choice;

    cout << "Enter source city (example City1): ";
    cin >> source;

    cout << "Enter destination city (example City50): ";
    cin >> destination;

    if (!graph.contains(source) || !graph.contains(destination)) {
        cerr << "Invalid city name.\n";
        return 1;
    }

    cout << "\nChoose routing criterion:\n";
    cout << "1. Fastest route\n";
    cout << "2. Cheapest route\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice != 1 && choice != 2) {
        cerr << "Invalid choice.\n";
        return 1;
    }

    bool useTime = (choice == 1);
    auto [totalWeight, path] =
        graph.shortestPath(source, destination, useTime);

    if (path.empty()) {
        cout << "No route found.\n";
        return 0;
    }

    cout << "\nBest path: ";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i) cout << " -> ";
        cout << path[i];
    }

    cout << "\nTotal " << (useTime ? "time" : "cost")
         << ": " << totalWeight << "\n";

    return 0;
}
