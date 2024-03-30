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
    void insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n);
    void removeFix(AVLNode<Key, Value> *n, int diff);
    void rotateLeft(AVLNode<Key, Value> *n);
    void rotateRight(AVLNode<Key, Value> *n);

    void fixLeftImbalance(AVLNode<Key, Value> *node, AVLNode<Key, Value> *parent, int newDiff);
    void fixDoubleRightImbalance(AVLNode<Key, Value> *node, AVLNode<Key, Value> *child, AVLNode<Key, Value> *parent, int newDiff);
    void fixRightImbalance(AVLNode<Key, Value> *node, AVLNode<Key, Value> *parent, int newDiff);
    void fixDoubleLeftImbalance(AVLNode<Key, Value> *node, AVLNode<Key, Value> *child, AVLNode<Key, Value> *parent, int newDiff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value> *node = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
    node->setBalance(0);    

    if (this->root_ == NULL) {
    this->root_ = node;
} else {
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (true) {
        if (current->getKey() == node->getKey()) {
            current->setValue(new_item.second);
            delete node; // Properly deallocate the memory for the node
            return;
        }
        if (new_item.first < current->getKey() && current->getLeft() != NULL) {
            current = current->getLeft();
        } else if (new_item.first > current->getKey() && current->getRight() != NULL) {
            current = current->getRight();
        } else {
            break;
        }
    }

    if (new_item.first < current->getKey()) {
        node->setParent(current);
        current->setLeft(node);
    } else {
        node->setParent(current);
        current->setRight(node);
    }

    node->setBalance(0);

    if (current->getBalance() != 0) {
        current->setBalance(0);
    } else {
        insertFix(current, node);
    }
}

}



/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value> *nodeToRemove = static_cast<AVLNode<Key, Value> *>(this->internalFind(key));
    if (nodeToRemove == nullptr)
        return;

    if (nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
        AVLNode<Key, Value> *predecessor = static_cast<AVLNode<Key, Value> *>(this->predecessor(nodeToRemove));
        nodeSwap(nodeToRemove, predecessor);
    }

    AVLNode<Key, Value> *parent = nodeToRemove->getParent();
    int difference = 0;
    if (parent != nullptr) {
        if (parent->getLeft() == nodeToRemove)
            difference = 1;
        else
            difference = -1;
    }

    AVLNode<Key, Value> *child = (nodeToRemove->getLeft() == nullptr) ? nodeToRemove->getRight() : nodeToRemove->getLeft();
    if (child != nullptr) {
        if (parent == nullptr) {
            this->root_ = child;
            child->setParent(nullptr);
        } else {
            if (parent->getLeft() == nodeToRemove)
                parent->setLeft(child);
            else
                parent->setRight(child);
            child->setParent(parent);
        }
    } else {
        if (parent == nullptr)
            this->root_ = nullptr;
        else {
            if (parent->getLeft() == nodeToRemove)
                parent->setLeft(nullptr);
            else
                parent->setRight(nullptr);
        }
    }
    removeFix(parent, difference);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


template <class K, class V>
void AVLTree<K, V>::insertFix(AVLNode<K, V> *parent, AVLNode<K, V> *newNode) {
    if (parent == nullptr || parent->getParent() == nullptr)
        return;

    AVLNode<K, V> *grandParent = parent->getParent();
    if (grandParent->getLeft() == parent) {
        grandParent->updateBalance(1);
        if (grandParent->getBalance() == 0)
            return;
        if (grandParent->getBalance() == -1)
            insertFix(grandParent, parent);
        else {
            if (newNode->getLeft() == parent)
                rotateRight(grandParent);
            else {
                rotateLeft(parent);
                rotateRight(grandParent);
            }
        }
    }
    else {
        grandParent->updateBalance(1);
        if (grandParent->getBalance() == 0)
            return;
        if (grandParent->getBalance() == 1)
            insertFix(grandParent, parent);
        else {
            if (newNode->getRight() == parent)
                rotateLeft(grandParent);
            else {
                rotateRight(parent);
                rotateLeft(grandParent);
            }
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value> *node, int diff) {
    if (node == nullptr)
        return;

    int newDiff = 0;
    AVLNode<Key, Value> *parent = node->getParent();
    if (parent != nullptr) {
        newDiff = (parent->getLeft() == node) ? 1 : -1;
    }

    int balanceDiff = node->getBalance() + diff;
    if (balanceDiff == -2) {
        fixLeftImbalance(node, parent, newDiff);
    } else if (balanceDiff == 2) {
        fixRightImbalance(node, parent, newDiff);
    } else if (balanceDiff == 0) {
        node->setBalance(0);
        removeFix(parent, newDiff);
    } else {
        node->setBalance(balanceDiff);
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::fixLeftImbalance(AVLNode<Key, Value> *node, AVLNode<Key, Value> *parent, int newDiff) {
    AVLNode<Key, Value> *child = node->getRight();
    if (child->getBalance() == -1) {
        rotateRight(node);
        node->setBalance(0);
        child->setBalance(0);
        removeFix(parent, newDiff);
    } else if (child->getBalance() == 0) {
        // Do nothing
    } else {
        fixDoubleRightImbalance(node, child, parent, newDiff);
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::fixDoubleRightImbalance(AVLNode<Key, Value> *node, AVLNode<Key, Value> *child,
                                                  AVLNode<Key, Value> *parent, int newDiff) {
    AVLNode<Key, Value> *grandChild = child->getRight();
    rotateLeft(child);
    rotateRight(node);
    if (grandChild->getBalance() == 0) {
        node->setBalance(0);
        child->setBalance(0);
        grandChild->setBalance(0);
    } else if (grandChild->getBalance() == 1) {
        node->setBalance(0);
        child->setBalance(-1);
        grandChild->setBalance(0);
    } else {
        node->setBalance(1);
        child->setBalance(0);
        grandChild->setBalance(0);
    }
    removeFix(parent, newDiff);
}

template <class Key, class Value>
void AVLTree<Key, Value>::fixRightImbalance(AVLNode<Key, Value> *node, AVLNode<Key, Value> *parent, int newDiff) {
    AVLNode<Key, Value> *child = node->getLeft();
    if (child->getBalance() == 1) {
        rotateLeft(node);
        node->setBalance(0);
        child->setBalance(0);
        removeFix(parent, newDiff);
    } else if (child->getBalance() == 0) {
        // Do nothing
    } else {
        fixDoubleLeftImbalance(node, child, parent, newDiff);
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::fixDoubleLeftImbalance(AVLNode<Key, Value> *node, AVLNode<Key, Value> *child,AVLNode<Key, Value> *parent, int newDiff) {
    AVLNode<Key, Value> *grandChild = child->getLeft();
    rotateRight(child);
    rotateLeft(node);
    if (grandChild->getBalance() == 0) {
        node->setBalance(0);
        child->setBalance(0);
        grandChild->setBalance(0);
    } else if (grandChild->getBalance() == -1) {
        node->setBalance(0);
        child->setBalance(1);
        grandChild->setBalance(0);
    } else {
        node->setBalance(-1);
        child->setBalance(0);
        grandChild->setBalance(0);
    }
    removeFix(parent, newDiff);
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *node) {
    AVLNode<Key, Value> *parentNode = node->getParent();
    AVLNode<Key, Value> *childNode = node->getRight();
    AVLNode<Key, Value> *grandChildNode = childNode->getLeft();
    
    if (parentNode == nullptr) {
        this->root_ = childNode;
        childNode->setParent(nullptr);
    } else {
        if (parentNode->getLeft() == node)
            parentNode->setLeft(childNode);
        else
            parentNode->setRight(childNode);
        childNode->setParent(parentNode);
    }
    
    childNode->setLeft(node);
    node->setParent(childNode);
    node->setRight(grandChildNode);
    if (grandChildNode != nullptr)
        grandChildNode->setParent(node);
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *node) {
    AVLNode<Key, Value> *parentNode = node->getParent();
    AVLNode<Key, Value> *childNode = node->getLeft();
    AVLNode<Key, Value> *grandChildNode = childNode->getRight();
    
    if (parentNode == nullptr) {
        this->root_ = childNode;
        childNode->setParent(nullptr);
    } else {
        if (parentNode->getLeft() == node)
            parentNode->setLeft(childNode);
        else
            parentNode->setRight(childNode);
        childNode->setParent(parentNode);
    }
    
    childNode->setRight(node);
    node->setParent(childNode);
    node->setLeft(grandChildNode);
    if (grandChildNode != nullptr)
        grandChildNode->setParent(node);
}


#endif