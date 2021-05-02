#include <iostream>
#include <unordered_set>
#include "digraph.h"

using namespace std;

unordered_set<int> reachability(const Digraph& graph, int v) {
  unordered_set<int> reached = {v};
  unordered_set<int> unexplored = {v};
  
  while (unexplored.size()>0) {
    int u = *unexplored.begin();

    // remove one node from the unexplored set
    unexplored.erase(unexplored.begin()); 
    for (auto iter = graph.neighbours(u); iter != graph.endIterator(u); iter++) {
      if (reached.find(*iter) == reached.end()) // not reached before
      {
        reached.insert(*iter);
        unexplored.insert(*iter);
      }
    }
  }

  return reached;
}

int main() {
  Digraph graph;

  int n, m;
  cin >> n >> m;
  for (int i = 0; i < n; ++i) {
    int label;
    cin >> label;
    graph.addVertex(label);
  }
  for (int i = 0; i < m; ++i) {
    int u, v;
    cin >> u >> v;
    graph.addEdge(u, v);
  }
  int vertex;
  cin >> vertex;

  // solve the graph reachability problem
  unordered_set<int> reachable_set = reachability(graph, vertex);

  // print the subset of all vertices reachable from 'vertex'
  cout << "Set of vertices reachable from " << vertex << ":" << endl;
  for (unordered_set<int>::iterator it = reachable_set.begin(); it != reachable_set.end(); ++it) {
    cout << (*it) << endl;
  }
  cout << endl;

  return 0;
}
