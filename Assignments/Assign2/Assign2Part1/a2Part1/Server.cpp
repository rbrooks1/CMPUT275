/*
Name: Ryan Brooks, Charlie Chen
ID: 1530605, 1530488
Course: CmPUT 275 winter 2019
assignment: Assignment 2 part 1: driving route finder
*/
#include "wdigraph.h"
#include "dijkstra.h"
#include <iostream>
#include <unordered_map>
#include <list>
#include <fstream>
#include <utility>
#include "math.h"
#include "digraph.h"

// constant declaration
#define INF 446744073709551614

using namespace std;

// struct declaration 
struct Point {
    long long lat;
    long long lon;
};

// function that calculates the manhattan distance between two points
long long manhattan(Point& pt1, Point& pt2) {
    return (abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon));
}

// this function creates the weighted directional graph and
// adds the costs to each edge.
void readGraph(string filename, WDigraph& graph, unordered_map<int, Point>& points) {
    /*
    Read the Edmonton map data from the provided fileand load it into
    the given WDigraph object.Store vertex coordinates in Point struct
    and map each vertex to its corresponding Point struct.
    PARAMETERS:
    filename: name of the file describing a road networkgraph: an instance of the weighted
    directed graph (WDigraph) classpoints: a mapping between vertex identifiers
    and their coordinates
    */
    ifstream file;
    string line;
    file.open(filename);
    while (getline(file, line)) {
        if (line[0] == 'V') {
            Point point;
            // find each instance of the a comma in the CSV
            int val1 = line.find(',', 2);
            int val2 = line.find(',', val1 + 1);
            int val3 = line.find(',', val2 + 11);
            // finding the longitudes and latitudes
            // and converts them to floats and finally
            // casts them to long longs
            string imVal1 = line.substr(val1 + 1, val2 - (val1 + 1));
            string imVal2 = line.substr(val2 + 1, val3 - (val2 + 1));
            float latitude = stof(imVal1);
            float longitude = stof(imVal2);
            latitude = static_cast<long long> (latitude * 100000);
            longitude = static_cast<long long> (longitude * 100000);
            // find sthe vertex label and adds it to the graph
            string imVal3 = line.substr(2, val1 - 2);
            int label = stoi(imVal3);
            point.lat = latitude;
            point.lon = longitude;
            points.insert({label, point});
            graph.addVertex(label);
        } else if (line[0] == 'E') {
            // find the two vertex indentifiers to put together
            int val1 = line.find(',', 2);
            int val2 = line.find(',', val1 + 1);
            string label1 = line.substr(2, val1 - 2);
            string label2 = line.substr(val1 + 1, val2 - (val1 + 1));
            int edge1 = stoi(label1);
            int edge2 = stoi(label2);
            // find the labels in the unordered map and use the latitutde
            // and longitude and use the manhattan distance to create the cost.
            auto find1 = points.find(edge1);
            auto find2 = points.find(edge2);
            long long cost = manhattan(find1->second, find2->second);
            graph.addEdge(edge1, edge2, cost);
        }
    }
    file.close();
}



// main function
int main() {
    string textFile = "edmonton-roads-2.0.1.txt";
    WDigraph graph;
    unordered_map<int, Point> map;
    readGraph(textFile, graph, map);
    char id;
    cin >> id;
    if (id == 'R') {
        // variable declaration and finding the longitudes
        // and latitudes of the starting and ending points
        long long lat1, lon1, lat2, lon2;
        cin >> lat1 >> lon1 >> lat2 >> lon2;
        Point sPoint;
        Point ePoint;
        sPoint.lat = lat1;
        sPoint.lon = lon1;
        ePoint.lat = lat2;
        ePoint.lon = lon2;
        unordered_map<long long, int> svalmap;
        unordered_map<long long, int> evalmap;
        // finds the between each vertex and the starting and ending
        // points
        for (auto p : map) {
            int label = p.first;
            long long sabsdist = manhattan(sPoint, p.second);
            svalmap[sabsdist] = label;

            long long eabsdist = manhattan(ePoint, p.second);
            evalmap[eabsdist] = label;
        }
        long long smin = INF;
        // finds the closest vertex to the starting point
        for (auto v : svalmap) {
            if (v.first < smin) {
                smin = v.first;
            }
        }
        int startVertex = svalmap.find(smin)->second;
        cout << startVertex << endl;
        long long emin = INF;
        // finds the closest vertex to the ending point
        for (auto v : evalmap) {
            if (v.first < emin) {
                emin = v.first;
            }
        }
        cout << "points" << endl;
        // finding the shortest path from the start vertex to the
        // end vertex
        int endVertex = evalmap.find(emin)->second;
        cout << endVertex << endl;
        unordered_map<int, PLI> tree;
        dijkstra(graph, startVertex, tree);
        vector<int> path;
        int w = endVertex;
        cout << endVertex << endl;
        path.push_back(w);
        while (w != startVertex) {
            int o = tree.find(w)->second.second;
            w = o;
            cout << w << endl;
            path.push_back(w);
        }
        int nodes = path.size();
        cout << 'N' << ' ' << nodes << "\n";
        // prints out each consecutive waypoint from the starting
        // vertex to the ending vertex
        while (nodes != 0) {
            char wId;
            cin >> wId;
            if (wId == 'A') {
                int wPoint = nodes - 1;
                int way = path[wPoint];
                Point outPoint = map.find(way)->second;
                cout << 'W' << outPoint.lat << ' ' << outPoint.lon << "\n";
                nodes--;
            }
        }
            cout << "E\n";
    }
    return 0;
}
    

