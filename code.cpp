
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

const int INF = 1e9;

struct Edge {
    int to, cap, flow, rev;
};

vector<vector<Edge>> adj;
vector<int> level;
vector<int> ptr;

void add_edge(int from, int to, int cap) {
    adj[from].push_back({to, cap, 0, (int)adj[to].size()});
    adj[to].push_back({from, cap, 0, (int)adj[from].size() - 1});
}

bool bfs(int s, int t) {
    fill(level.begin(), level.end(), -1);
    level[s] = 0;
    queue<int> q;
    q.push(s);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (auto& edge : adj[v]) {
            if (edge.cap - edge.flow > 0 && level[edge.to] == -1) {
                level[edge.to] = level[v] + 1;
                q.push(edge.to);
            }
        }
    }
    return level[t] != -1;
}

int dfs(int v, int t, int pushed) {
    if (pushed == 0) return 0;
    if (v == t) return pushed;
    for (int& cid = ptr[v]; cid < adj[v].size(); ++cid) {
        auto& edge = adj[v][cid];
        int tr = edge.to;
        if (level[v] + 1 != level[tr] || edge.cap - edge.flow == 0) continue;
        int push = dfs(tr, t, min(pushed, edge.cap - edge.flow));
        if (push == 0) continue;
        edge.flow += push;
        adj[tr][edge.rev].flow -= push;
        return push;
    }
    return 0;
}

int dinic(int s, int t) {
    int flow = 0;
    for (auto& v : adj) {
        for (auto& e : v) {
            e.flow = 0;
        }
    }
    while (bfs(s, t)) {
        fill(ptr.begin(), ptr.end(), 0);
        while (int pushed = dfs(s, t, INF)) {
            flow += pushed;
        }
    }
    return flow;
}

struct GHEdge {
    int u, v, w;
};

struct DSU {
    vector<int> parent;
    vector<int> sz;
    DSU(int n) {
        parent.resize(n + 1);
        sz.resize(n + 1, 1);
        for (int i = 1; i <= n; ++i) parent[i] = i;
    }
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            if (sz[root_i] < sz[root_j]) swap(root_i, root_j);
            parent[root_j] = root_i;
            sz[root_i] += sz[root_j];
        }
    }
};

int main() {
    int n, m;
    if (!(cin >> n >> m)) return 0;
    adj.resize(n + 1);
    level.resize(n + 1);
    ptr.resize(n + 1);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        add_edge(u, v, 1);
    }

    vector<int> p(n + 1, 1);
    vector<GHEdge> gh_edges;
    for (int i = 2; i <= n; ++i) {
        int s = i, t = p[i];
        int flow = dinic(s, t);
        gh_edges.push_back({s, t, flow});
        vector<bool> in_s(n + 1, false);
        queue<int> q;
        q.push(s);
        in_s[s] = true;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (auto& edge : adj[v]) {
                if (edge.cap - edge.flow > 0 && !in_s[edge.to]) {
                    in_s[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }
        for (int j = i + 1; j <= n; ++j) {
            if (p[j] == t && in_s[j]) {
                p[j] = i;
            }
        }
    }

    sort(gh_edges.begin(), gh_edges.end(), [](const GHEdge& a, const GHEdge& b) {
        return a.w > b.w;
    });

    DSU dsu(n);
    long long total_flow = 0;
    for (auto& edge : gh_edges) {
        total_flow += (long long)edge.w * dsu.sz[dsu.find(edge.u)] * dsu.sz[dsu.find(edge.v)];
        dsu.unite(edge.u, edge.v);
    }

    cout << total_flow << endl;

    return 0;
}
