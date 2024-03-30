#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError
{
};

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
    AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value> *getParent() const override;
    virtual AVLNode<Key, Value> *getLeft() const override;
    virtual AVLNode<Key, Value> *getRight() const override;

protected:
    int8_t balance_; // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
 * An explicit constructor to initialize the elements by calling the base class constructor
 */
template <class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent) : Node<Key, Value>(key, value, parent), balance_(0)
{
}

/**
 * A destructor which does nothing.
 */
template <class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{
}

/**
 * A getter for the balance of a AVLNode.
 */
template <class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
 * A setter for the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
 * Adds diff to the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
 * An overridden function for getting the parent since a static_cast is necessary to make sure
 * that our node is a AVLNode.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value> *>(this->parent_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value> *>(this->left_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value> *>(this->right_);
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
    virtual void insert(const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key &key);                              // TODO
protected:
    virtual void nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n);
    void removeFix(AVLNode<Key, Value> *n, int diff);
    void rotateLeft(AVLNode<Key, Value> *n);
    void rotateRight(AVLNode<Key, Value> *n);
};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    Key key = new_item.first;
    Value val = new_item.second;
    AVLNode<Key, Value> *node = new AVLNode<Key, Value>(key, val, NULL);
    if (this->root_ == NULL)
    {
        this->root_ = static_cast<Node<Key, Value> *>(node);
        return;
    }
    AVLNode<Key, Value> *p = static_cast<AVLNode<Key, Value> *>(this->root_);
    while (true)
    {
        if (p->getKey() == key)
        {
            p->setValue(val);
            return;
        }
        if ((key < p->getKey()) & (p->getLeft() != NULL))
            p = p->getLeft();
        else if ((key > p->getKey()) & (p->getRight() != NULL))
            p = p->getRight();
        else
            break;
    }
    // now curr is the parent of the node to be insert
    if (key < p->getKey())
    {
        node->setParent(p);
        p->setLeft(node);
    }
    else
    {
        node->setParent(p);
        p->setRight(node);
    }
    node->setBalance(0);
    if (p->getBalance() != 0)
        p->setBalance(0);
    else
    {
        if (p->getLeft() == node)
            p->updateBalance(-1);
        else
            p->updateBalance(1);
        insertFix(p, node);
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n)
{
    if (p == NULL || p->getParent() == NULL)
        return;
    AVLNode<Key, Value> *g = p->getParent();
    if (g->getLeft() == p)
    {
        g->updateBalance(-1);
        if (g->getBalance() == 0)
            return;
        if (g->getBalance() == -1)
            insertFix(g, p);
        else
        {
            if (p->getLeft() == n)
            {
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else
            {
                rotateLeft(p);
                rotateRight(g);

                if (n->getBalance() == -1)
                {
                    p->setBalance(0);
                    g->setBalance(1);
                    n->setBalance(0);
                }
                else if (n->getBalance() == 0)
                {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                else
                {
                    p->setBalance(-1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }
    }
    else
    {
        g->updateBalance(1);
        if (g->getBalance() == 0)
            return;
        if (g->getBalance() == 1)
            insertFix(g, p);
        else
        {
            if (p->getRight() == n)
            {
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else
            {
                rotateRight(p);
                rotateLeft(g);

                if (n->getBalance() == 1)
                {
                    p->setBalance(0);
                    g->setBalance(-1);
                    n->setBalance(0);
                }
                else if (n->getBalance() == 0)
                {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                else
                {
                    p->setBalance(1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::remove(const Key &key)
{
    AVLNode<Key, Value> *n = static_cast<AVLNode<Key, Value> *>(this->internalFind(key));
    if (n == NULL)
        return;
    if (n->getLeft() != NULL && n->getRight() != NULL)
    {
        AVLNode<Key, Value> *succ = static_cast<AVLNode<Key, Value> *>(this->predecessor(n));
        nodeSwap(n, succ);
    }
    AVLNode<Key, Value> *p = n->getParent();
    int diff = 0;
    if (p != NULL)
    {
        if (p->getLeft() == n)
            diff = 1;
        else
            diff = -1;
    }
    AVLNode<Key, Value> *c = n->getLeft() == NULL ? n->getRight() : n->getLeft();
    if (c != NULL)
    {
        if (p == NULL)
        {
            this->root_ = c;
            c->setParent(NULL);
        }
        else
        {
            if (p->getLeft() == n)
                p->setLeft(c);
            else
                p->setRight(c);
            c->setParent(p);
        }
    }
    else
    {
        if (p == NULL)
            this->root_ = NULL;
        else
        {
            if (p->getLeft() == n)
                p->setLeft(NULL);
            else
                p->setRight(NULL);
        }
    }
    delete n;
    removeFix(p, diff);
}

template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value> *n, int diff)
{
    if (n == NULL)
        return;
    int newDiff = 0;
    AVLNode<Key, Value> *p = n->getParent();
    if (p != NULL)
    {
        if (p->getLeft() == n)
            newDiff = 1;
        else
            newDiff = -1;
    }
    if (n->getBalance() + diff == -2)
    {
        AVLNode<Key, Value> *c = n->getLeft();
        if (c->getBalance() == -1)
        {
            rotateRight(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, newDiff);
        }
        else if (c->getBalance() == 0)
        {
            rotateRight(n);
            n->setBalance(-1);
            c->setBalance(1);
            return;
        }
        else
        {
            AVLNode<Key, Value> *gc = c->getRight();
            rotateLeft(c);
            rotateRight(n);
            if (gc->getBalance() == 0)
            {
                n->setBalance(0);
                c->setBalance(0);
                gc->setBalance(0);
            }
            else if (gc->getBalance() == 1)
            {
                n->setBalance(0);
                c->setBalance(-1);
                gc->setBalance(0);
            }
            else
            {
                n->setBalance(1);
                c->setBalance(0);
                gc->setBalance(0);
            }
            removeFix(p, newDiff);
        }
    }
    else if (n->getBalance() + diff == 2)
    {
        // flip left/right and -1/+1
        AVLNode<Key, Value> *c = n->getRight();
        if (c->getBalance() == 1)
        {
            rotateLeft(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, newDiff);
        }
        else if (c->getBalance() == 0)
        {
            rotateLeft(n);
            n->setBalance(1);
            c->setBalance(-1);
            return;
        }
        else
        {
            AVLNode<Key, Value> *gc = c->getLeft();
            rotateRight(c);
            rotateLeft(n);
            if (gc->getBalance() == 0)
            {
                n->setBalance(0);
                c->setBalance(0);
                gc->setBalance(0);
            }
            else if (gc->getBalance() == 1)
            {
                n->setBalance(0);
                c->setBalance(0);
                gc->setBalance(0);
            }
            else
            {
                n->setBalance(0);
                c->setBalance(1);
                gc->setBalance(0);
            }
            removeFix(p, newDiff);
        }
    }
    else if (n->getBalance() + diff == 0)
    {
        n->setBalance(0);
        removeFix(p, newDiff);
    }
    else
        n->setBalance(n->getBalance() + diff);
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *n)
{
    AVLNode<Key, Value> *p = n->getParent();
    AVLNode<Key, Value> *c = n->getRight();
    AVLNode<Key, Value> *gc = c->getLeft();
    if (p == NULL)
    {
        this->root_ = c;
        c->setParent(NULL);
    }
    else
    {
        if (p->getLeft() == n)
            p->setLeft(c);
        else
            p->setRight(c);
        c->setParent(p);
    }
    c->setLeft(n);
    n->setParent(c);
    n->setRight(gc);
    if (gc != NULL)
        gc->setParent(n);
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *n)
{
    AVLNode<Key, Value> *p = n->getParent();
    AVLNode<Key, Value> *c = n->getLeft();
    AVLNode<Key, Value> *gc = c->getRight();
    if (p == NULL)
    {
        this->root_ = c;
        c->setParent(NULL);
    }
    else
    {
        if (p->getLeft() == n)
            p->setLeft(c);
        else
            p->setRight(c);
        c->setParent(p);
    }
    c->setRight(n);
    n->setParent(c);
    n->setLeft(gc);
    if (gc != NULL)
        gc->setParent(n);
}

template <class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif