/*
Name: Ryan Brooks, Charlie Chen
ID: 1530605, 1530488
Course: CmPUT 275 winter 2019
assignment: Assignment 2 part 1: driving route finder
*/
#include "dijkstra.h"
#include "digraph.h"
#include "heap.h"
#include <utility>
#include <iostream>
#include <unordered_map>

// finds the shortest distance from the starting vertex to any
// others vertex in the graph.
void dijkstra (const WDigraph& graph, int startVertex, unordered_map<int, PLI>& tree) {
    BinaryHeap<pair<int, int>, long long> events;
    pair<int,int> p(startVertex, startVertex);
    events.insert(p, 0);
    cout << startVertex << endl;
    while (events.size() > 0) {
        cout << events.size() << endl;
        cout << "loop" << endl;
        int u = events.min().first.first;
        cout << " stcuk" << endl;
        int v = events.min().first.second;
        cout << " stcuk" << endl;
        long long time = events.min().second;
        cout << " stcuk" << endl;
        events.popMin();
        cout << events.size() << endl;
        if (tree.find(v) == tree.end()) {
            tree[v] = {time, u};
            cout << "v: "<< v << endl;

            for (auto neighbour = graph.neighbours(v); neighbour != graph.endIterator(v); neighbour++){
                auto w = *neighbour;
                pair<int,int> p2(v, w);
                time += graph.getCost(v,w);
                events.insert(p2, time);
                cout << "w: "<< w << endl;
            }

        }
    }
}