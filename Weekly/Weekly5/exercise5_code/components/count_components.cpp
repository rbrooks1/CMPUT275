/*
Name: Ryan Brooks
ID: 1530605
Course: CMPUT 275 winter 2019
assignment: Weekly exercise 5: graph concepts
*/
#include "digraph.h"
#include "digraph.cpp"
#include <iostream>
#include <unordered_map>
#include <list>
#include <fstream>

using namespace std;

// global variable declaration
bool hasBeenSeen;

// this function was taken from eclass
// this function recursively traverses the graph and
// finds all the connected components
void depthFirstSearch(const Digraph& graph, int v, int parent, unordered_map<int, int>& searchTree) {
  if (searchTree.find(v) != searchTree.end()) {
    hasBeenSeen = true;
    return;  // already visited this node
  }

  searchTree[v] = parent;
  for (auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
    depthFirstSearch(graph, *iter, v, searchTree);
  }
  hasBeenSeen = false;
}

// function that reads a text file using comma
// seperated lists and creates a graph class
// using the vertices and edges defined in the
// text file.
Digraph read_city_graph_undirected(string filename) {
    // create variables and open the file for reading
    ifstream file;
    string line;
    Digraph graph;
    file.open(filename);
    while (getline(file, line)) {
        // addes vertices
        if (line[0] == 'V') {
            int val = line.find(',', 2);
            string label = line.substr(2, val - 2);
            graph.addVertex(stoul(label));
        } else if (line[0] == 'E') {  // adds edges
            int val1 = line.find(',', 2);
            int val2 = line.find(',', val1 + 1);
            string label1 = line.substr(2, val1 - 2);
            string label2 = line.substr(val1 + 1, val2 - (val1 + 1));
            uint32_t edge1 = stoul(label1);
            uint32_t edge2 = stoul(label2);
            graph.addEdge(edge1, edge2);
            graph.addEdge(edge2, edge1);
        }
    }
    // close the file and return thr created graph
    file.close();
    return graph;
}

// this function uses depthFirstSearch to count
// all the connected components in the graph passed
// to it
int count_components(Digraph g) {
    unordered_map<int, int> map;
    vector<int> vertex = g.vertices();
    int components = 0;
    for (auto v : vertex) {
        depthFirstSearch(g, v, -1, map);
        if (hasBeenSeen == false) {
            components += 1;
        }
    }
    return components;
}

// main function
int main() {
    string textFile = "edmonton-roads-2.0.1.txt";
    Digraph graph = read_city_graph_undirected(textFile);
    int count = count_components(graph);
    cout << count << endl;
    return 0;
}
