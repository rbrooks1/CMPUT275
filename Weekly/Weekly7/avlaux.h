/*
Name: Ryan Brooks
ID: 1530605
Course: CMPUT 275 winter 2019
assignment: Weekly exercise 7: AVL trees
*/
#ifndef _AVL_AUX_H_
#define _AVL_AUX_H_

#include <cstdlib>

// forward declaration of class, so AVLNode can establish it's "friends" :)
template <typename K, typename T> class AVLMap;
template <typename K, typename T> class AVLIterator;

/*
Node for holding the key, item, and pointers for a node in the AVL.
Everything is private, only AVLMap abd AVLIterator have access.
*/
template <typename K, typename T>
class AVLNode {
private:
  AVLNode(const K& key, const T& item,
    AVLNode<K,T>* left, AVLNode<K,T>* right, AVLNode<K,T>* parent) {
      this->key = key;
      this->item = item;
      this->left = left;
      this->right = right;
      this->parent = parent;
      recalcHeight();
  }

  // recursively delete the left and right subtree, if they exist
  ~AVLNode() {
    if (this->left) {
      delete this->left;
    }
    if (this->right) {
      delete this->right;
    }
  }

  // recalculate the height of this node
  // assumes the heights of the children are correct
  void recalcHeight() {
    int lh, rh;
    childHeights(lh, rh);
    height = 1+std::max(lh, rh);
  }

  // get the heights of the children
  void childHeights(int& lh, int& rh) {
    lh = rh = -1;
    if (left) {
      lh = left->height;
    }
    if (right) {
      rh = right->height;
    }
  }

  K key;
  T item;
  AVLNode<K,T> *left, *right, *parent;
  int height;
  int val;

  // give access to the AVL map class itself and its iterators
  friend class AVLMap<K,T>;
  friend class AVLIterator<K,T>;
};

/*
Iterator class for the AVLMap class.

Supports:
- key()
- item(), as an l-value as well
- prefix and postfix increment
- == and !=
*/
template <typename K, typename T>
class AVLIterator {
public:
  const K& key() const {
    return this->node->key;
  }

  // allows assignment to the item, eg. iter.item() = 17
  // this will update the item held at by the corresponding key

  const T& item() const {
    return this->node->item;
  }

  T& item(){
    return this->node->item;
  }

  // prefix operator: ++iter
  AVLIterator<K,T> operator++() {
    advance();
    return *this;
  }

  // postfix operator: iter++
  AVLIterator<K,T>& operator++(int) {
    // uses the default copy constructor to copy this->node
    AVLIterator<K,T> tmp(*this);
    advance();
    return tmp;
  }

  bool operator==(const AVLIterator<K,T>& rhs) const {
    return node == rhs.node;
  }

  bool operator!=(const AVLIterator<K,T>& rhs) const {
    return node != rhs.node;
  }

private:
  AVLIterator(AVLNode<K,T> *root) {
    this->node = root;
    if (node != NULL) {
      // if the root of the tree is not empty, go to leftmost node
      while (this->node->left) {
        this->node = node->left;
      }
    }
  }

  void advance() {
    assert(node != NULL);
    if (this->node->right) {
      // if there is a right subtree, go to its leftmost/minimum-key node
      this->node = this->node->right;
      while (this->node->left) {
        this->node = this->node->left;
      }
    }
    else {
      // crawl up parent pointers while this node is the right child of the parent
      const AVLNode<K,T> *old;
      do {
        old = this->node;
        this->node = this->node->parent;
      } while (this->node != NULL && this->node->right == old);
    }
  }

  AVLNode<K,T> *node;

  // needed so AVLMap can access the constructor
  friend class AVLMap<K,T>;
};


  #endif
