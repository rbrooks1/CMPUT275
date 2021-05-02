#include <iostream>
#include <unordered_map>
#include <list>
#include "digraph.h"

using namespace std;

void depthFirstSearch(const Digraph& graph, int v, int parent, unordered_map<int, int>& searchTree) {
  if (searchTree.find(v) != searchTree.end()) {
    return; // already visited this node
  }

  searchTree[v] = parent;

  for (auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
    depthFirstSearch(graph, *iter, v, searchTree);
  }
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
  int startVertex, endVertex;

  cin >> startVertex >> endVertex;

  // do the DFS to get the search tree
  unordered_map<int, int> searchTree;
  depthFirstSearch(graph, startVertex, -1, searchTree);

  // print searchTree
  cout << "Search tree:" << endl;
  for (auto it : searchTree) {
    cout << it.first << ": " << it.second << endl;
  }
  cout << endl;

  // get a path from startVertex to endVertex
  list<int> path;
  if (searchTree.find(endVertex) == searchTree.end()) {
    cout << "Vertex " << endVertex << " not reachable from " << startVertex << endl;
  }
  else {
    int stepping = endVertex;
    while (stepping != startVertex) {
      path.push_front(stepping);
      stepping = searchTree[stepping]; // crawl up the search tree one step at a time
    }
    path.push_front(startVertex);

    cout << "Path from " << startVertex << " to " << endVertex << ":";
    for (auto it : path) {
      cout << ' ' << it;
    }
    cout << endl;
  }

  return 0;
}
