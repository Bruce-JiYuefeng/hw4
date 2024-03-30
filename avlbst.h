#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value> *n);
    void rotateRight(AVLNode<Key, Value> *n);
    void insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n);
    void removeFix(AVLNode<Key, Value> *n, int diff);
    void updateBalancesAfterDoubleRotation(AVLNode<Key, Value> *node, AVLNode<Key, Value> *child, AVLNode<Key, Value> *grandChild);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    Key key = new_item.first;
    Value value = new_item.second;
    
    // If tree is empty, create root and return.
    if (!this->root_) {
        this->root_ = new AVLNode<Key, Value>(key, value, nullptr);
        return;
    }

    // Traverse the tree to find the insertion point.
    AVLNode<Key, Value> *parent = static_cast<AVLNode<Key, Value> *>(this->root_);
    while (true) {
        // If key already exists, update value and return.
        if (key == parent->getKey()) {
            parent->setValue(value);
            return;
        }
        
        // Navigate left or right based on key comparison.
        if (key < parent->getKey()) {
            if (parent->getLeft()) {
                parent = parent->getLeft();
            } else {
                // Found insertion point on the left.
                AVLNode<Key, Value> *newNode = new AVLNode<Key, Value>(key, value, parent);
                parent->setLeft(newNode);
                break;
            }
        } else {
            if (parent->getRight()) {
                parent = parent->getRight();
            } else {
                // Found insertion point on the right.
                AVLNode<Key, Value> *newNode = new AVLNode<Key, Value>(key, value, parent);
                parent->setRight(newNode);
                break;
            }
        }
    }

    // Fix AVL tree balance if needed.
    insertFix(parent);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value> *targetNode = static_cast<AVLNode<Key, Value> *>(this->internalFind(key));
    if (!targetNode) return;

    // If the target node has two children, swap it with its predecessor.
    if (targetNode->getLeft() && targetNode->getRight()) {
        AVLNode<Key, Value> *predecessor = static_cast<AVLNode<Key, Value> *>(this->predecessor(targetNode));
        nodeSwap(targetNode, predecessor);
    }

    AVLNode<Key, Value> *parentNode = targetNode->getParent();
    AVLNode<Key, Value> *childNode = targetNode->getLeft() ? targetNode->getLeft() : targetNode->getRight();

    // Adjust parent pointers.
    if (childNode) {
        childNode->setParent(parentNode);
    }
    
    if (!parentNode) {
        this->root_ = childNode;
    } else {
        // Update the parent's child pointer.
        if (parentNode->getLeft() == targetNode) {
            parentNode->setLeft(childNode);
        } else {
            parentNode->setRight(childNode);
        }
    }

    // Calculate balance factor difference for potential fixes.
    int balanceDiff = parentNode ? (parentNode->getLeft() == targetNode ? 1 : -1) : 0;
    
    // Fix the AVL tree balance if needed.
    removeFix(parentNode, balanceDiff);
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *node) {
    AVLNode<Key, Value> *parent = node->getParent();
    AVLNode<Key, Value> *newRoot = node->getRight();
    AVLNode<Key, Value> *newRootLeftChild = newRoot->getLeft();

    // Make newRoot the root of the subtree.
    if (!parent) {
        this->root_ = newRoot;
        newRoot->setParent(nullptr);
    } else {
        if (parent->getLeft() == node) parent->setLeft(newRoot);
        else parent->setRight(newRoot);
        newRoot->setParent(parent);
    }

    // Reassign children.
    newRoot->setLeft(node);
    node->setParent(newRoot);
    node->setRight(newRootLeftChild);
    if (newRootLeftChild) newRootLeftChild->setParent(node);
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *node) {
    AVLNode<Key, Value> *parent = node->getParent();
    AVLNode<Key, Value> *newRoot = node->getLeft();
    AVLNode<Key, Value> *newRootRightChild = newRoot->getRight();

    // Make newRoot the root of the subtree.
    if (!parent) {
        this->root_ = newRoot;
        newRoot->setParent(nullptr);
    } else {
        if (parent->getLeft() == node) parent->setLeft(newRoot);
        else parent->setRight(newRoot);
        newRoot->setParent(parent);
    }

    // Reassign children.
    newRoot->setRight(node);
    node->setParent(newRoot);
    node->setLeft(newRootRightChild);
    if (newRootRightChild) newRootRightChild->setParent(node);
}

template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value> *node, int balanceChange)
{
    if (!node) return; // Base case: reached root or empty node.

    AVLNode<Key, Value> *parent = node->getParent();
    int parentDirection = 0;
    if (parent) {
        parentDirection = (parent->getLeft() == node) ? 1 : -1; // Determine node's direction relative to its parent.
    }

    // Calculate new balance after removal.
    int newBalance = node->getBalance() + balanceChange;

    // Case 1: Left heavy imbalance.
    if (newBalance == -2) {
        AVLNode<Key, Value> *leftChild = node->getLeft();
        int leftChildBalance = leftChild->getBalance();

        if (leftChildBalance <= 0) {
            // Left-Left Case (Single right rotation).
            rotateRight(node);
            node->setBalance(0);
            leftChild->setBalance(0);
            if (leftChildBalance == -1) removeFix(parent, parentDirection); // Continue fixing up the tree.
        } else {
            // Left-Right Case (Double rotation: left then right).
            AVLNode<Key, Value> *grandChild = leftChild->getRight();
            rotateLeft(leftChild);
            rotateRight(node);
            updateBalancesAfterDoubleRotation(node, leftChild, grandChild);
            removeFix(parent, parentDirection); // Continue fixing up the tree.
        }
    }
    // Case 2: Right heavy imbalance.
    else if (newBalance == 2) {
        AVLNode<Key, Value> *rightChild = node->getRight();
        int rightChildBalance = rightChild->getBalance();

        if (rightChildBalance >= 0) {
            // Right-Right Case (Single left rotation).
            rotateLeft(node);
            node->setBalance(0);
            rightChild->setBalance(0);
            if (rightChildBalance == 1) removeFix(parent, parentDirection); // Continue fixing up the tree.
        } else {
            // Right-Left Case (Double rotation: right then left).
            AVLNode<Key, Value> *grandChild = rightChild->getLeft();
            rotateRight(rightChild);
            rotateLeft(node);
            updateBalancesAfterDoubleRotation(node, rightChild, grandChild);
            removeFix(parent, parentDirection); // Continue fixing up the tree.
        }
    }
    // Case 3: The balance is zero after removal - height of the subtree has decreased.
    else if (newBalance == 0) {
        node->setBalance(0);
        removeFix(parent, parentDirection); // Continue fixing up the tree as the height has changed.
    }
    // Case 4: The balance is not critically imbalanced, simply update the balance.
    else {
        node->setBalance(newBalance);
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::updateBalancesAfterDoubleRotation(AVLNode<Key, Value> *node, AVLNode<Key, Value> *child, AVLNode<Key, Value> *grandChild) {
    switch (grandChild->getBalance()) {
        case -1:
            node->setBalance(0);
            child->setBalance(1);
            break;
        case 1:
            node->setBalance(-1);
            child->setBalance(0);
            break;
        default:
            node->setBalance(0);
            child->setBalance(0);
            break;
    }
    grandChild->setBalance(0); // Grandchild becomes balanced after double rotation.
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}



#endif
