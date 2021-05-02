/*
Name: Ryan Brooks, Charlie Chen
ID: 1530605, 1530488
Course: CmPUT 275 winter 2019
assignment: Assignment 2 part 1: driving route finder
*/
#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

#include "wdigraph.h"

typedef pair<long long, int> PLI;

// declaration for dijkstra.cpp
void dijkstra (const WDigraph& graph, int startVertex, unordered_map<int, PLI>& tree);

#endif