#include <iostream>
#include <vector>
#include <set>
#include <stack>

using namespace std;

class Graph
{
public:
    int vertices;
    vector<vector<int>> adjacencyList;
    set<vector<int>> startNode;
    set<vector<int>> endNode;

    Graph(int v) : vertices(v), adjacencyList(v) {}

    void addEdge(int u, int v)
    {
        adjacencyList[u].push_back(v);
    }

    bool findend(int start, vector<int> path, vector<bool> &visited)
    {
        for (int neighbor : adjacencyList[start])
        {
            if (!visited[neighbor])
                return true;
        }

        return false;
    }

    void
    findAllCyclesUtil(int first, int v, vector<bool> &visited, vector<int> &path)
    {
        visited[v] = true;
        path.push_back(v);

        for (const auto &neighbor : adjacencyList[v])
        {
            if (!visited[neighbor])
            {
                findAllCyclesUtil(first, neighbor, visited, path);
            }
            else
            {
                int start = findIndex(path, neighbor);

                bool possibleend = findend(start, path, visited);
                int i;
                vector<int> ep;
                vector<int> sp;
                cout << "Cycle found: ";
                for (i = start; i < path.size(); ++i)
                {
                    cout << path[i] + 1 << " ";

                    if (path.size() - start > 2)
                    {
                        if (path[start] != first)
                        {
                            ep.push_back(path[i]);
                        }

                        if (possibleend && i != start)
                            sp.push_back(path[i]);
                    }
                }

                if (possibleend && path.size() - start > 2)
                    sp.push_back(path[start]);

                cout << path[start] + 1 << endl;

                endNode.insert(ep);
                startNode.insert(sp);
            }
        }

        visited[v] = false;
        path.pop_back();
    }

    int findIndex(const vector<int> &path, int vertex)
    {
        for (int i = 0; i < path.size(); ++i)
        {
            if (path[i] == vertex)
            {
                return i;
            }
        }
        return -1;
    }

    void findAllCycles()
    {
        vector<bool> visited(vertices, false);
        vector<int> path;

        for (int i = 0; i < vertices; ++i)
        {
            findAllCyclesUtil(i, i, visited, path);
        }
    }

    void printPath(const vector<int> &path, vector<int> startV, vector<int> endV)
    {
        if (path.size() > 0)
        {
            cout << "Simple Path: ";

            // cout << "start = ";
            for (int i = 0; i < startV.size() - 1; i++)
                cout << startV[i] + 1 << " ";

            // cout << "\nfind = ";
            for (int index = 0; index < path.size() - 1; index++)
                cout << path[index] + 1 << " ";

            // cout << "\nend = ";
            for (int i = 0; i < endV.size(); i++)
                cout << endV[i] + 1 << " ";

            cout << endl;
        }
    }

    void findAllSimplePaths(int start, int end, vector<bool> visited, vector<int> startV, vector<int> endV)
    {
        vector<int> path;
        vector<bool> visit(visited.size());

        copy(visited.begin(), visited.end(), visit.begin());

        // for (bool v : visit)
        // {
        //     cout << v << " ";
        // }

        // cout << endl;

        findAllSimplePathsUtil(start, end, visit, path, startV, endV);
    }

    void findAllSimplePathsUtil(int current, int end, vector<bool> &visited, vector<int> &path, vector<int> startV, vector<int> endV)
    {
        visited[current] = true;
        path.push_back(current);

        if (current == end)
        {
            // Print the current path
            printPath(path, startV, endV);
        }
        else
        {
            for (int neighbor : adjacencyList[current])
            {
                if (!visited[neighbor])
                {
                    findAllSimplePathsUtil(neighbor, end, visited, path, startV, endV);
                }
            }
        }

        // Backtrack
        visited[current] = false;
        path.pop_back();
    }
};

int main()
{

    int N, E;
    cin >> N >> E;

    Graph graph(N);

    vector<int> startNode(N, 0);
    vector<int> endNode(N, 0);

    int start, end;
    for (int i = 0; i < E; i++)
    {
        cin >> start >> end;
        graph.addEdge(start - 1, end - 1);
        startNode[start - 1]++;
        endNode[end - 1]++;
    }

    for (int i = 0; i < N; i++)
    {
        if (startNode[i] == 0)
            end = i;
        if (endNode[i] == 0)
            start = i;
    }

    vector<int> SP, EP;
    SP.push_back(start);
    EP.push_back(end);
    graph.startNode.insert(SP);
    graph.endNode.insert(EP);
    graph.findAllCycles();

    int startIndex = 0;
    int endIndex = 0;

    for (vector<int> s : graph.startNode)
    {
        if (s.size() > 0)
        {

            int se = s.back();

            for (vector<int> e : graph.endNode)
            {
                if (e.size() > 0)
                {
                    vector<bool> visits(N);

                    for (int j = 0; j < s.size(); j++)
                        visits[s[j]] = true;
                    for (int a = 1; a < e.size(); a++)
                        visits[e[a]] = true;

                    int es = e.front();

                    graph.findAllSimplePaths(se, es, visits, s, e);

                    // for (int node : s)
                    // {
                    //     cout << node + 1 << " ";
                    // }
                    // cout << "/";

                    // for (int node : e)
                    // {
                    //     cout << node + 1 << " ";
                    // }

                    // cout << endl;
                }
            }
        }
    }

    return 0;
}
