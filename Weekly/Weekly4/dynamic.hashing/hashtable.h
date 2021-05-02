/*
Name: Ryan Brooks
ID: 1530605
Course: CMPUT 275 winter 2019
assignment: Weekly exercise 4: dynamic hashing and makefile
*/
#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include "linkedlist.h"
#include <cassert>

// An iterator. With more time, we could make this "safer" by
// making the members private but visible to the HashTable class
// by using "friend".
template <typename T>
struct HashTableIterator {
  unsigned int bucket;
  ListNode<T> *node;

  T item() const {
    return node->item;
  }
};

/*
  A hash table for storing items. It is assumed the type T of the item
  being stored has a hash method, eg. you can call item.hash(), which
  returns an unsigned integer.

  Also assumes the != operator is implemented for the item being stored,
  so we could check if two items are different same.

  If you just want store integers, wrap them up in a struct
  with a .hash() method and an != operator.
*/

template <typename T>
class HashTable {
public:
  // creates an empty hash table of the given size.
  HashTable(unsigned int tableSize) {
    assert(tableSize > 0);

    table = new LinkedList<T>[tableSize];
    assert(table != NULL);

    this->tableSize = tableSize;
    numItems = 0;
}
  // second constructor in the case where no
  // value is passed into the class creation call
  // in the .cpp file.
  HashTable() {
    assert(tableSize > 0);
    unsigned int tableSize = 10;
    table = new LinkedList<T>[tableSize];
    assert(table != NULL);

    this->tableSize = tableSize;
    numItems = 0;
  }

  ~HashTable() {
    // will call the destructor for each list in the table
    delete[] table;
  }

  // Check if the item already appears in the table.
  bool contains(const T& item) const;

  // Insert the item, do nothing if it is already in the table.
  // Returns true iff the insertion was successful (i.e. the item was not there).
  bool insert(const T& item);

  // Removes the item after checking, via assert, that the item was in the table.
  void remove(const T& item);

  // Returns the number of items held in the table.
  unsigned int size() const { return numItems; }

  void reallocate();



  // Useful methods for iteration.

  // Get the iterator for the first item.
  HashTableIterator<T> startIterator() const;

  // Advance the iterator (returns the next iterator).
  HashTableIterator<T> nextIterator(const HashTableIterator<T>& iter) const;

  // Test if a given iterator is the end iterator.
  bool isEndIterator(const HashTableIterator<T>& iter) const;

private:
  LinkedList<T> *table;
  unsigned int numItems;
  unsigned int tableSize;

  // Computes which hash table bucket the item maps to.
  unsigned int getBucket(const T& item) const {
    return item.hash() % tableSize;
  }
};

template <typename T>
bool HashTable<T>::contains(const T& item) const {
  // just get the item's bucket and use the lists find feature
  unsigned int hashVal = getBucket(item);
  return (table[hashVal].find(item) != NULL);
}

template <typename T>
bool HashTable<T>::insert(const T& item) {
  // get the item's bucket
  unsigned int bucket = getBucket(item);

  // If the item is not in this bucket, insert it.
  // Use insertFront, it always takes O(1) time.
  if (table[bucket].find(item) == NULL) {
    table[bucket].insertFront(item);
    numItems++;
    if (numItems >= tableSize) {
        reallocate();
    }
    return true;
  }
  else {
    // the item was already in the table
    return false;
  }
}

template <typename T>
void HashTable<T>::remove(const T& item) {
  unsigned int bucket = getBucket(item);

  ListNode<T> *node = table[bucket].find(item);
  assert(node != NULL); // make sure the item was in the table

  table[bucket].removeNode(node);
  numItems--;
}

template <typename T>
HashTableIterator<T> HashTable<T>::startIterator() const {
  HashTableIterator<T> iter;
  if (numItems == 0) {
    // indicates this is after all items
    iter.bucket = tableSize;
    iter.node = NULL;
  }
  else {
    // find the first nonempty bucket
    for (iter.bucket = 0; table[iter.bucket].size() == 0; iter.bucket++);
    // and point to the first item in this bucket
    iter.node = table[iter.bucket].getFirst();
  }
  return iter;
}

template <typename T>
HashTableIterator<T> HashTable<T>::nextIterator(const HashTableIterator<T>& iter) const {
  HashTableIterator<T> next = iter;

  next.node = next.node->next;

  // This bucket is done, move on to the next nonempty bucket or
  // to the end of the buckets (i.e. end iterator).
  if (next.node == NULL) {
    next.bucket++;

    // Iterate until you reach another nonempty bucket or the end of buckets
    while (next.bucket < tableSize && table[next.bucket].size() == 0) {
      next.bucket++;
    }

    // If we ended at a nonempty bucket, point to the first item.
    if (next.bucket < tableSize) {
      next.node = table[next.bucket].getFirst();
    }
    // Otherwise, this is the end iterator so let's point to NULL to be safe.
    else {
      next.node = NULL;
    }
  }
  return next;
}

template <typename T>
bool HashTable<T>::isEndIterator(const HashTableIterator<T>& iter) const {
  // We use iter.bucket == tableSize to represent the end iterator.
  return (iter.bucket == tableSize);
}

// This function runs only if the number of items in
// the original hashtable are of the same size as the
// table and it creates a new instance of the hashtable
// and moves all values from the old hashtable to the new one
// then deallocates the memory of the old hashtable.
template <typename T>
void HashTable<T>::reallocate() {
  unsigned int newTableSize = 2*tableSize;
  LinkedList<T> *newTable = new LinkedList<T>[newTableSize];
  // iterate through all instances of the old table and
  // get the bucket value from the old table then insert the
  // value into the new table
 HashTableIterator<T> start = startIterator();
 while (true) {
    unsigned int bucketVal = getBucket(start.node->item);
    newTable[bucketVal].insertFront(start.node->item);
    HashTableIterator<T> next = nextIterator(start);
    start = next;
    if (isEndIterator(start) == true) {
        break;
    }
  }
  // deallocate memory for the old table and then point
  // the new table to be the old table as well as update
  // the table size.
  delete[] table;
  tableSize = newTableSize;
  table = newTable;
  }

#endif
