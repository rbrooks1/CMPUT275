#include <iostream>
#include <unordered_map>
#include <list>
#include <fstream>
#include <utility>
#include "heap.h"

using namespace std;


int main() {
    BinaryHeap<int, int> B;
    B.insert(1, 2);
    B.popMin();

    return 0;
}