/*
Name: Ryan Brooks, Charlie Chen
ID: 1530605, 1530488
Course: CmPUT 275 winter 2019
assignment: Assignment 2 part 2: Client
Credits: In README
*/
#include <iostream>
#include <list>
#include "wdigraph.h"
#include "dijkstra.h"
#include "server_util.h"
#include "serialport.h"
#include <deque>
#include <vector>
#include <string>
#include <cassert>
#include <cstdlib>

using namespace std;

SerialPort serial("/dev/ttyACM0");
int k = 0;
// taken in input as an array of subStrings from the serial
// Inputs:
// subS(string[]): array of substrings.
void waitForInput(string subS[]) {
    string inS = serial.readline();
    for (int i = 0; i < 5; i++) {
        subS[i] = "";
    } 
        int at = 0;
        for (auto c : inS) {
            if (at == 5) {
                break;
            }
          if ((c == ' ') || (c == '\n')) {
            // start new string
            ++at;
          }
          else {
            // append character to the string we are building
            subS[at] += c;
          }
          // don't want to index out of bounds!
        }
}

// finds the shortest path from the start vertex to the end vertex
// Input:
// subS(string): an array of substrings
// points(unordered_map): contains the coordinates of all the vertices and their id
// graph(WDigraph): A weighted directed graph that contains the vertices and edges of the edmonton map
// vert(deque): a deque containing all vertices in the shortest path from start vertex to end vertex
void mode0(string subS[], unordered_map<int, Point> points, WDigraph graph, vector<int> vert) {
    Point sPoint, ePoint;
    // creates start and end coordinates
    sPoint.lat = stoll(subS[1]);
    sPoint.lon = stoll(subS[2]);
    ePoint.lat = stoll(subS[3]);
    ePoint.lon = stoll(subS[4]);
    // get the points closest to the two input points
    int start = findClosest(sPoint, points), end = findClosest(ePoint, points);

    // run dijkstra's to compute a shortest path
    unordered_map<int, PLI> tree;
    dijkstra(graph, start, tree);
    if (tree.find(end) == tree.end()) {
        // no path
        serial.writeline("N 0\n");
    } else {
        // read off the path by stepping back through the search tree
        list<int> path;
        while (end != start) {
            path.push_front(end);
            end = tree[end].second;
        }
        path.push_front(start);
        // adds path to a deque
        for (auto v : path) {
            vert.push_back(v);
        }
        int wsize = vert.size();
        string w = to_string(wsize);
        serial.writeline("N ");
        serial.writeline(w);
        serial.writeline("\n");
    }
}

// resets the array of strings, and the deque
// Inputs:
// subS(string): array of strings
// vert(deque): a deque containing the vertices of the path.
void reset(string subS[], vector<int> vert) {
    vert.clear();
    // clears the subS
    for (int i = 0; i < 5; i++) {
        subS[i] = "";
    }
}


// sends one waypoint for the path to the arduino from the front of the deque.
// Inputs: vert(vector<int>): vector holding all the vertex identifiers
// points(unordered_map): contains the vertex identifiers with their coordinates.

void wayP(vector<int> vert, unordered_map<int, Point> points) {
    if (k < vert.size()) {
        long long v1 = points[vert[k]].lat;
        long long v2 = points[vert[k]].lon;
        string w1 = to_string(v1);
        string w2 = to_string(v2);
        // prints the waypoint to the serial connection
        serial.writeline("W");
        serial.writeline(" ");
        serial.writeline(w1);
        serial.writeline(" ");
        serial.writeline(w2);
        serial.writeline("\n");
        // next vertex in the path
        k++;
    } else if (k == vert.size()) {
        // end of path
        serial.writeline("E");
        serial.writeline("\n");
        k = 0;
        vert.clear();
    }
    
}

// reads the map and handles communication
int main() {
    // vector holding vertex identifiers
    vector<int> vert;
    WDigraph graph;
    unordered_map<int, Point> points;

    // build the graph
    readGraph("edmonton-roads-2.0.1.txt", graph, points);
    // read a request
    string subS[5];
    while (true) {
        waitForInput(subS);
        if ((subS[0][0] == 'R')) {
            mode0(subS, points, graph, vert);
        } else if (subS[0][0] == 'A') {
            wayP(vert, points);
            cout << 'A' << endl;
        }
        /* else if (subS[0][0] = NULL) {
            reset(subS, vert);
            cout << "timeout" << endl;
        }*/
    }
  return 0;
}
