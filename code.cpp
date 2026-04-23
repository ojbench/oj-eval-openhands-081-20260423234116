

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>

using namespace std;

const int MAXN = 3005;

struct Edge {
    int to, rev;
    int cap;
};

vector<Edge> adj[MAXN];
int n, m;

void add_edge(int u, int v) {
    adj[u].push_back({v, (int)adj[v].size(), 1});
    adj[v].push_back({u, (int)adj[u].size() - 1, 1});
}

int parent_node[MAXN];
int parent_edge[MAXN];
int q[MAXN];

int find_path(int s, int t) {
    for(int i=1; i<=n; ++i) parent_node[i] = 0;
    int head = 0, tail = 0;
    q[tail++] = s;
    parent_node[s] = -1;
    while(head < tail){
        int u = q[head++];
        if(u == t) return 1;
        for(int i=0; i<(int)adj[u].size(); ++i){
            auto& e = adj[u][i];
            if(e.cap > 0 && parent_node[e.to] == 0){
                parent_node[e.to] = u;
                parent_edge[e.to] = i;
                q[tail++] = e.to;
            }
        }
    }
    return 0;
}

int max_flow(int s, int t) {
    for (int i = 1; i <= n; ++i) {
        for (auto& e : adj[i]) e.cap = 1;
    }
    int flow = 0;
    while(find_path(s, t)){
        flow++;
        int curr = t;
        while(curr != s){
            int prev = parent_node[curr];
            int idx = parent_edge[curr];
            adj[prev][idx].cap--;
            int rev_idx = adj[prev][idx].rev;
            adj[curr][rev_idx].cap++;
            curr = prev;
        }
        if(flow == 3) break;
    }
    return flow;
}

struct GHEdge {
    int u, v, w;
};

struct DSU {
    int parent[MAXN];
    int sz[MAXN];
    DSU(int n) {
        for (int i = 1; i <= n; ++i) {
            parent[i] = i;
            sz[i] = 1;
        }
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
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    if (!(cin >> n >> m)) return 0;
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        add_edge(u, v);
    }

    vector<int> p(n + 1, 1);
    vector<GHEdge> gh_edges;
    for (int i = 2; i <= n; ++i) {
        int s = i, t = p[i];
        int flow = max_flow(s, t);
        gh_edges.push_back({s, t, flow});
        
        for(int k=1; k<=n; ++k) parent_node[k] = 0;
        int head = 0, tail = 0;
        q[tail++] = s;
        parent_node[s] = -1;
        while(head < tail){
            int u = q[head++];
            for(auto& e : adj[u]){
                if(e.cap > 0 && parent_node[e.to] == 0){
                    parent_node[e.to] = u;
                    q[tail++] = e.to;
                }
            }
        }

        for (int j = i + 1; j <= n; ++j) {
            if (p[j] == t && parent_node[j] != 0) {
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

