/*
Name: Ryan Brooks, Charlie Chen
ID: 1530605, 1530488
Course: CmPUT 275 winter 2019
assignment: Assignment 2 part 1: driving route finder
*/
#ifndef _HEAP_H_
#define _HEAP_H_

#include <vector>
#include <utility> // for pair
#include <algorithm>  
#include <iostream>

using namespace std;
// T is the type of the item to be held
// K is the type of the key associated with each item in the heap
// The only requirement is that K is totally ordered and comparable via <
template <class T, class K>
class BinaryHeap {
public:
  // constructor not required because the only "initialization"
  // is through the constructor of the variable "heap" which is called by default
    // get index of parent
    int parent(int i) {
        return (i-1)/2;
    }


    // left child
    int left(int i) {
        return (2*i + 1);
    }


    // right child
    int right(int i) {
        return (2*i + 2);
    }

  // return the minimum element in the heap
  pair<T, K> min() const;


  // insert an item with the given key
  // if the item is already in the heap, will still insert a new copy with this key
  void insert(const T& item, const K& key);

  // pop the minimum item from the heap
  void popMin();

  // returns the number of items held in the heap
  int size() const;

private:
  // the array holding the heap
  vector< pair<T, K> > heap;

  // feel free to add appropriate private methods to help implement some functions
};

// returns the minimum value in the heap
template <class T, class K>
pair<T, K> BinaryHeap<T, K>::min() const {
    return heap[0];
}

// function to swap two values
void swap(int *a, int *b) 
{ 
    int temp = *a; 
    *a = *b; 
    *b = temp; 
}

// insert a pair into the heap at the back
template <class T, class K>
void BinaryHeap<T, K>::insert(const T& item, const K& key) {
    pair<T, K> v(item, key);
    heap.push_back(v);
    int i = heap.size() - 1;
    // swaps only if the value of the parent is smaller then the value of the
    // child
    while ((i != 0) && (heap[parent(i)].second > heap[i].second)) {
        swap(heap[i], heap[parent(i)]);
        i = parent(i);
    }
}

// deletes the minimum value from the heap.
template <class T, class K>
void BinaryHeap<T, K>::popMin() {
    if (heap.size() == 1) {
        heap.pop_back();
    } else {
        int j = heap.size() - 1;
        swap(heap[0], heap[j]);
        heap.pop_back();
        pair<T, K> v = heap[0];
        int i = 0;
        j--;
        while ((i < j) && ((heap[left(i)].second < heap[i].second) || 
            (heap[right(i)].second < heap[i].second))) {
            cout << "i: " << heap[i].second << endl;
            cout << "left: " << heap[left(i)].second << endl;
            cout << "right: " << heap[right(i)].second << endl;
            if ((left(i) <= j) && (right(i) <= j)) {
                K m = std::min(heap[left(i)].second, heap[right(i)].second);
                if (m == heap[left(i)].second) {
                    swap(heap[i], heap[left(i)]);
                    i = left(i);
                    cout << "pop" << endl;
                } else if (m == heap[right(i)].second) {
                    swap(heap[i], heap[right(i)]);
                    i = right(i);
                } 
           } else if ((left(i) > j) && (right(i) <= j))  {
                if (heap[right(i)].second < heap[i].second) {
                    swap(heap[i], heap[right(i)]);
                    i = right(i);
                }
           } else if ((right(i) > j) && (left(i) <= j)) {
                if (heap[left(i)].second < heap[i].second) {
                    swap(heap[i], heap[left(i)]);
                    i = left(i);
                }
           }
        }
    }
    
}

// returns the size of the heap
template <class T, class K>
int BinaryHeap<T, K>::size() const{
    // cout << heap.size() << endl;
    return heap.size();
}



#endif
