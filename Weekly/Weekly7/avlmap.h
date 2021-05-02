/*
Name: Ryan Brooks
ID: 1530605
Course: CMPUT 275 winter 2019
assignment: Weekly exercise 7: AVL trees
*/
#ifndef _AVL_MAP_H_
#define _AVL_MAP_H_

#include <cassert>
#include <iostream>
#include <cstdlib>
#include "avlaux.h"

using namespace std;

/*
  An associative container (map/dict) using an AVL tree.
	The update, remove, [], at, and hasKey operations take O(log n)
	time (using O(log n) comparisons) where n = # entries
	in the tree.

  Assumes:
    - K is totally ordered and can be compared via <
*/

template <typename K, typename T>
class AVLMap {
public:
	// creates an empty AVLMap with 0 items
	AVLMap();

	// deletes all nodes in the AVLMap
	~AVLMap();

	// add the item with the given key, replacing
	// the old item at that key if the key already exists
	void update(const K& key, const T& item);

	// remove the key and its associated item
	void remove(const K& key);

	// returns true iff the key exists
	bool hasKey(const K& key) const;

	// access the item at the given key, allows assignment
	// as an l-value, eg. tree["Zac"] = 20;
	// where tree is an instance of AVLMap<string, int>
	T& operator[](const K& key);

	// does not create the entry if it does not exist
	const T& at(const K& key) const;

	// returns the size of the tree
	unsigned int size() const;

	// returns an iterator to the first item (ordered by key)
	AVLIterator<K,T> begin() const;

	// returns an iterator signalling the end iterator
	AVLIterator<K,T> end() const;

	// will run a check ensuring the AVL tree is still well-formed
	// (i.e. is a binary search tree with the AVL property at each node
	// and has the correct node count)
	// Takes O(n) time.
	void checkStructure() const;

	unsigned  int  countLessThan(const K& queryKey) const;

private:
	AVLNode<K,T> *root;
	unsigned int avlSize;

	// returns a pointer to the node containing the key,
	// or to what its parent node would be if the key does not exist,
	// or NULL if the tree is currently empty
	AVLNode<K,T>* findNode(const K& key) const;

	// assumes at least one child of node is NULL, will delete
	// the node and move its only child (if any) to its place
	void pluckNode(AVLNode<K,T>* node);

	// fix the AVL property at this node and, recursively, all
	// nodes above it
	void fixUp(AVLNode<K,T>* node);

	// recalculate the height of the node, assuming its children's heights
	// are correct
	void recalcHeight(AVLNode<K,T>* node);

	// left and right rotations, return a pointer to the new root
	// of the subtree after the rotation, assumes the corresponding
	// node->left or node->right are not null
	AVLNode<K,T>* rotateLeft(AVLNode<K,T>* node);
	AVLNode<K,T>* rotateRight(AVLNode<K,T>* node);

	// helper function for checkStructure
	unsigned int checkStructureRecursive(AVLNode<K,T> *node,
		AVLNode<K,T> *parent, AVLNode<K,T> *lb, AVLNode<K,T> *ub) const;
};

// ***** IMPLEMENTATION STARTS HERE *****

// ***** PUBLIC METHODS FIRST *****

template <typename K, typename T>
AVLMap<K,T>::AVLMap() {
	this->root = NULL;
	this->avlSize = 0;
}

template <typename K, typename T>
AVLMap<K,T>::~AVLMap() {
	if (this->root != NULL) {
		delete this->root;

		// point to NULL
		this->root = NULL;


		this->avlSize = 0;
	}
}

template <typename K, typename T>
void AVLMap<K,T>::update(const K& key, const T& item) {
	AVLNode<K,T>* node = findNode(key);

	// if there was no node in the tree with this key, create one
	if (node == NULL || node->key != key) {
		AVLNode<K,T> *newNode = new AVLNode<K,T>(key, item, NULL, NULL, node);
		assert(newNode != NULL);
		// initialize new node value to 1
		newNode->val = 1;
		// change the left or right pointer of the parent node
		// whichever is appropriate to preserve the AVL property
		if (node == NULL) {
			// the tree was empty, so this is the new root
			root = newNode;
		}
		else {
			// the tree was not empty, so put it as the appropriate child of "node"
			if (key < node->key) {
				node->left = newNode;
			}
			else {
				node->right = newNode;
			}
		}
		// update all the values of the parents after a new node is added
		AVLNode<K, T> *imVal = newNode;
		while (imVal->parent != NULL) {
			imVal->parent->val += 1;
			imVal = imVal->parent;
		}
		++avlSize;

		// now fix the AVL property up the tree
		fixUp(newNode);
	}
	else {
		// the key existed, so just update the item
		node->item = item;
	}
}

template <typename K, typename T>
void AVLMap<K,T>::remove(const K& key) {
	AVLNode<K,T>* node = findNode(key);

	// make sure the key is in the tree
	// we only assume < is implemented for the key type, not necessarily ==
	assert(node != NULL && !(node->key < key || key < node->key));

	// find the maximum-key node in the left subtree of the node to remove
	AVLNode<K,T> *tmp = node->left, *pluck = node;
	while (tmp != NULL) {
		pluck = tmp;
		tmp = tmp->right;
	}
	// pluck is the one to remove, it might be pluck == node if node has
	// no left child

	// copy contents from node we are deleting
	node->key = pluck->key;
	node->item = pluck->item;

	AVLNode<K,T> *pluckParent = pluck->parent;

	// this function will delete a node with no left child and
	// restructure the tree
	pluckNode(pluck);

	// now fix the AVL tree up starting from the parent
	// of the recently-deleted node
	fixUp(pluckParent);
}

template <typename K, typename T>
bool AVLMap<K,T>::hasKey(const K& key) const {

	// "find" the node, and then check it really has the key
	AVLNode<K,T> *node = findNode(key);
	return node != NULL && !(node->key != key);
}

template <typename K, typename T>
T& AVLMap<K,T>::operator[](const K& key) {

	// "find" the node, if not found then create an entry
	// using the default constructor for the item type
	if (!hasKey(key)) {
		update(key, T());
	}

	return findNode(key)->item;
}

template <typename K, typename T>
const T& AVLMap<K,T>::at(const K& key) const {
	const AVLNode<K,T> *node = findNode(key);
	assert(node != NULL && !(node->key != key));

	return node->item;
}

template <typename K, typename T>
unsigned int AVLMap<K,T>::size() const {
	return this->avlSize;
}

template <typename K, typename T>
AVLNode<K,T>* AVLMap<K,T>::findNode(const K& key) const {
	AVLNode<K,T> *node = this->root, *parent = NULL;

	// traverse down the tree, going left and right as appropriate,
	// until the key is found or we fall off of the leaf node
	while (node != NULL && node->key != key) {
		parent = node;
		if (key < node->key) {
			node = node->left;
		}
		else {
			node = node->right;
		}
	}

	if (node == NULL) {
		// key not found
		// here, parent == NULL if and only if the tree is empty
		return parent;
	}
	else {
		// key found
		return node;
	}
}

// an AVLIterator is just a wrapper for a pointer to a node
template <typename K, typename T>
AVLIterator<K,T> AVLMap<K,T>::begin() const {
	return AVLIterator<K,T>(this->root);
}

// the NULL pointer represents the end iterator
template <typename K, typename T>
AVLIterator<K,T> AVLMap<K,T>::end() const {
	return AVLIterator<K,T>(NULL);
}


// ***** NOW THE PRIVATE METHODS *****

template <typename K, typename T>
void AVLMap<K,T>::pluckNode(AVLNode<K,T>* node) {

	// first find the only child (if any) of "node"
	AVLNode<K,T> *child;
	if (node->left) {
		child = node->left;
		// make sure the node does not have two children
		assert(child->right == NULL);
	}
	else {
		// might still be NULL, meaning we are plucking a leaf node
		child = node->right;
	}

	// adjust the appropriate child pointer of the node's parent
	if (node->parent == NULL) {
		// in this case, we are deleting the root node
		// so set the new root to the child
		this->root = child;
	}
	else if (node->parent->left == node) {
		node->parent->left = child;
	}
	else {
		node->parent->right = child;
	}

	// if we are not deleting a leaf, the child also gets a new parent
	if (child) {
		child->parent = node->parent;
	}

	// ensures ~AVLNode() does not recursively delete other parts of the tree
	node->left = node->right = NULL;

	// decrement the value of each node after a node is removed
	AVLNode<K, T> *imVal = node;
	while (imVal->parent != NULL) {
		imVal->parent->val -= 1;
		imVal = imVal->parent;
	}
	// the node is now gone!
	delete node;
	--avlSize;
}

template <typename K, typename T>
void AVLMap<K,T>::fixUp(AVLNode<K,T> *node) {
	// keep climbing up the tree until we are past the root
	while (node != NULL) {
		// first make sure the height of node is correctly computed
		node->recalcHeight();

		// now compare the heights of the children
		int lh, rh;
		node->childHeights(lh, rh);

		// should never differ by more than 2, otherwise
		// there was a bug in the code
		assert(abs(lh-rh) <= 2);

		// if there is a violation of the AVL property, perform the
		// appropriate rotation(s)
		// see eClass notes for the different rules for applying rotations
		if (lh == rh+2) {
			// left child is higher

			AVLNode<K,T>* lchild = node->left;
			int llh, lrh;
			lchild->childHeights(llh, lrh);

			if (llh < lrh) {
				rotateLeft(lchild);
			}
			node = rotateRight(node);
		}
		else if (lh+2 == rh) {
			// right child is higher

			AVLNode<K,T>* rchild = node->right;
			int rlh, rrh;
			rchild->childHeights(rlh, rrh);

			if (rlh > rrh) {
				rotateRight(rchild);
			}
			node = rotateLeft(node);
		}

		// whether we rotated or not, "node" is now the
		// root of the subtree we were checking


		// crawl up the tree one step
		node = node->parent;
	}
}

template <typename K, typename T>
AVLNode<K,T>* AVLMap<K,T>::rotateRight(AVLNode<K,T>* node) {
	AVLNode<K,T> *lchild = node->left;
	assert(left != NULL);
	// store old values for future use
	int imVal1 = lchild->val;
	int imVal2 = 0;
	int imVal3 = node->val;
	if (lchild->right != NULL) {
		imVal2 = lchild->right->val;
	}
	// To track all of these changes, it is best to
	// draw your own picture with all relevant pointers
	// to see these operations change them properly.
	if (node->parent) {
		if (node == node->parent->left) {
			node->parent->left = lchild;
		}
		else {
			node->parent->right = lchild;
		}
	}
	else {
		root = lchild;
	}

	lchild->parent = node->parent;
	node->parent = lchild;
	if (lchild->right) {
		lchild->right->parent = node;
	}

	node->left = lchild->right;
	lchild->right = node;

	node->recalcHeight();
	lchild->recalcHeight();
	// update values using the stored old values
	node->val = imVal3 - (imVal1 - imVal2);
	lchild->val = imVal1 - imVal2 + node->val;

	return lchild;
}

template <typename K, typename T>
AVLNode<K,T>* AVLMap<K,T>::rotateLeft(AVLNode<K,T>* node) {
	AVLNode<K,T> *rchild = node->right;
	assert(left != NULL);
	// store old values for future use
	int imVal1 = rchild->val;
	int imVal2 = 0;
	int imVal3 = node->val;
	if (rchild->left != NULL) {
		imVal2 = rchild->left->val;
	}
	// To track all of these changes, it is best to
	// draw your own picture with all relevant pointers
	// to see these operations change them properly.

	if (node->parent) {
		if (node == node->parent->left) {
			node->parent->left = rchild;
		}
		else {
			node->parent->right = rchild;
		}
	}
	else {
		root = rchild;
	}

	rchild->parent = node->parent;
	node->parent = rchild;
	if (rchild->left) {
		rchild->left->parent = node;
	}

	node->right = rchild->left;
	rchild->left = node;

	node->recalcHeight();
	rchild->recalcHeight();
	// update values using stored old values
	node->val = imVal3 - (imVal1 - imVal2);
	rchild->val = imVal1 - imVal2 + node->val;
	return rchild;
}

template <typename K, typename T>
void AVLMap<K,T>::checkStructure() const {
	if (root == NULL) {
		assert(avlSize == 0);
	}
	else {
		assert(root->parent == NULL);
		unsigned int checkSize = checkStructureRecursive(root, NULL, NULL, NULL);
		assert(checkSize == avlSize);
	}
}

// Recursively:
// - checks the AVL height property in all nodes the subtree rooted at "node"
// - calculates and returns the number of nodes in this subtree
// - the height of the nodes are correct
// - ensures the BST property at each node in the subtree holds, see
//   the inline comments to understand how this is done
template <typename K, typename T>
unsigned int AVLMap<K,T>::checkStructureRecursive(AVLNode<K,T> *node,
	AVLNode<K,T> *parent, AVLNode<K,T> *lb, AVLNode<K,T> *ub) const {

	unsigned int cnt = 1;

	// Think: lb means "lower bound" and ub means "upper bound".
	//   lb is the deepest node above "node" such that "node" is
	//   in the right subtree of lb, similarly for ub (except left subtree)
	//   So if node->key and some key in a node above "node" violate the BST
	//   but no violation has been detected so far earlier in the recursion
	//   then it node->key would violate the BST property with one of ub or lb
	if (lb != NULL) {
		assert(lb->key < node->key);
	}
	if (ub != NULL) {
		assert(node->key < ub->key);
	}

	// the following recursively checks
	if (node->left) {
		assert(node->left->parent == node);
		// replace ub with "node" in the recursive check as we went left
		cnt += checkStructureRecursive(node->left, node, lb, node);
	}
	if (node->right) {
		assert(node->right->parent == node);
		// replace lb with "node" in the recursive check as we went right
		cnt += checkStructureRecursive(node->right, node, node, ub);
	}

	// now check the heights and the AVL property
	int lh, rh;
	node->childHeights(lh, rh);
	assert(node->height == 1+max(lh, rh));
	assert(abs(lh-rh) <= 1);

	return cnt;
}

//function taht finds all the node values that are less then
// a given queryKey
template <typename K, typename T>
unsigned int AVLMap<K,T>::countLessThan(const K& queryKey) const {
	// initialize the root and a count
	AVLNode<K, T> *node = this->root;
	unsigned int count = 0;
	// run until the node is null
	while (node != NULL) {
		// increment the count if the key of the node is less then
		// the query key
		if (node->key < queryKey) {
			count++;
			// increase the count by the value of the left node
			if (node->left != NULL) {
				count += node->left->val;
			}
			// move right along the tree
			node = node->right;
		// otherwise move left along the tree
		} else if (node->key >= queryKey) {
			node = node->left;
		}
	}
	return count;
}

#endif
