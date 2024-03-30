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
    void rotateLeft(AVLNode<Key, Value>* n);
    void rotateRight(AVLNode<Key, Value>* n);
    void insertFix(AVLNode<Key, Value>* n, AVLNode<Key, Value>* p);
    void removeFix(AVLNode<Key, Value>* n, int diff);
    void insertLeft(const std::pair<const Key, Value> &new_item, AVLNode<Key, Value> *parent);
    void insertRight(const std::pair<const Key, Value> &new_item, AVLNode<Key, Value> *parent);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item) {
    if (this->root_ == nullptr) {
        AVLNode<Key, Value>* new_root = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        new_root->setBalance(0);
        new_root->setLeft(nullptr);
        new_root->setRight(nullptr);
        this->root_ = new_root;
        return;
    }

    AVLNode<Key, Value>* current_node = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (current_node != nullptr) {
        if (current_node->getKey() > new_item.first) {
            if (current_node->getLeft() != nullptr) {
                current_node = current_node->getLeft();
            } else {
                insertLeft(new_item, current_node);
                return;
            }
        } else if (current_node->getKey() < new_item.first) {
            if (current_node->getRight() != nullptr) {
                current_node = current_node->getRight();
            } else {
                insertRight(new_item, current_node);
                return;
            }
        } else { 
            current_node->setValue(new_item.second);
            return;
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertLeft(const std::pair<const Key, Value> &new_item, AVLNode<Key, Value>* parent) {
    AVLNode<Key, Value>* new_node = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    parent->setLeft(new_node);
    new_node->setParent(parent);
    new_node->setBalance(0);

    if (parent->getBalance() == 1 || parent->getBalance() == -1) {
        parent->setBalance(0);
    } else if (parent->getBalance() == 0) {
        parent->updateBalance(-1);
        insertFix(new_node, parent);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertRight(const std::pair<const Key, Value> &new_item, AVLNode<Key, Value>* parent) {
    AVLNode<Key, Value>* new_node = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    parent->setRight(new_node);
    new_node->setParent(parent);
    new_node->setBalance(0);

    if (parent->getBalance() == 1 || parent->getBalance() == -1) {
        parent->setBalance(0);
    } else if (parent->getBalance() == 0) {
        parent->updateBalance(1);
        insertFix(new_node, parent);
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    AVLNode<Key, Value>* rmNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (rmNode == nullptr) {
        return; 
    }

    if (rmNode->getLeft() != nullptr && rmNode->getRight() != nullptr) {
        AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key, Value>*>(this->predecessor(rmNode));
        nodeSwap(rmNode, predecessor);
    }

    AVLNode<Key, Value>* child_node = (rmNode->getRight() != nullptr) ? rmNode->getRight() : rmNode->getLeft();

    if (child_node != nullptr) {
        child_node->setParent(rmNode->getParent());
    }

    AVLNode<Key, Value>* parent_node = rmNode->getParent();
    int diff = 0; 
    if (parent_node == nullptr) {
        this->root_ = child_node;
    } else {
        if (rmNode == parent_node->getLeft()) {
            parent_node->setLeft(child_node);
            diff = 1;
        } else {
            parent_node->setRight(child_node);
            diff = -1;
        }
    }

    delete rmNode;

    removeFix(parent_node, diff);
}


template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* currentNode, int balanceAdjustment) {
  if (currentNode == nullptr) return;

  AVLNode<Key, Value>* parentNode = currentNode->getParent();
  int parentAdjustment = (parentNode != nullptr && currentNode == parentNode->getLeft()) ? 1 : -1;

  // Common function to handle rotations and rebalancing
  auto handleRotation = [&](AVLNode<Key, Value>* childNode, int childBalanceCondition, auto rotationFunc, bool doubleRotation = false, AVLNode<Key, Value>* grandChildNode = nullptr) {
    if (childNode->getBalance() == childBalanceCondition) {
      rotationFunc(currentNode);
      int zeroBalance = 0;
      currentNode->setBalance(zeroBalance);
      childNode->setBalance(zeroBalance);
      if (!doubleRotation) removeFix(parentNode, parentAdjustment);
    } else if (doubleRotation && grandChildNode) {
      int childNewBalance = 0, currentNodeNewBalance = 0;
      rotationFunc(childNode);
      rotationFunc(currentNode);
      switch (grandChildNode->getBalance()) {
        case 1: currentNodeNewBalance = 0; childNewBalance = -1; break;
        case -1: currentNodeNewBalance = 1; childNewBalance = 0; break;
        default: currentNodeNewBalance = childNewBalance = 0;
      }
      currentNode->setBalance(currentNodeNewBalance);
      childNode->setBalance(childNewBalance);
      grandChildNode->setBalance(0);
      removeFix(parentNode, parentAdjustment);
    } else {
      int adjustment = (childBalanceCondition == -1) ? 1 : -1;
      rotationFunc(currentNode);
      currentNode->setBalance(adjustment);
      childNode->setBalance(-adjustment);
    }
  };

  int newBalance = currentNode->getBalance() + balanceAdjustment;
  if (balanceAdjustment == -1) {
    AVLNode<Key, Value>* leftChild = currentNode->getLeft();
    if (newBalance == -2) {
      if (leftChild->getBalance() == 1) {
        handleRotation(leftChild, 1, [&](AVLNode<Key, Value>* node) { rotateLeft(leftChild); rotateRight(node); }, true, leftChild->getRight());
      } else {
        handleRotation(leftChild, leftChild->getBalance(), [&](AVLNode<Key, Value>* node) { rotateRight(node); });
      }
    } else {
      currentNode->setBalance(newBalance);
      if (newBalance != -1) removeFix(parentNode, parentAdjustment);
    }
  } else if (balanceAdjustment == 1) {
    AVLNode<Key, Value>* rightChild = currentNode->getRight();
    if (newBalance == 2) {
      if (rightChild->getBalance() == -1) {
        handleRotation(rightChild, -1, [&](AVLNode<Key, Value>* node) { rotateRight(rightChild); rotateLeft(node); }, true, rightChild->getLeft());
      } else {
        handleRotation(rightChild, rightChild->getBalance(), [&](AVLNode<Key, Value>* node) { rotateLeft(node); });
      }
    } else {
      currentNode->setBalance(newBalance);
      if (newBalance != 1) removeFix(parentNode, parentAdjustment);
    }
  }
}


template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* n, AVLNode<Key, Value>* p)
{
  if(p == NULL || p->getParent() == NULL) {
    return;
  }
  AVLNode<Key, Value>* g = p->getParent();
  if(g->getLeft() == p) {
    g->updateBalance(-1);
    if(g->getBalance() == 0) return;
    else if (g->getBalance() == -1) {
      insertFix(p, g);
    }
    else if (g->getBalance() == -2) {
      if(n == p->getLeft()) {
        rotateRight(g);
        p->setBalance(0);
        g->setBalance(0);
      }
      else if (n == p->getRight()) {
        rotateLeft(p);
        rotateRight(g);
        if(n->getBalance() == -1) {
          p->setBalance(0);
          g->setBalance(1);
        }
        else if (n->getBalance() == 0) {
          p->setBalance(0);
          g->setBalance(0);
        }
        else if (n->getBalance() == 1) {
          p->setBalance(-1);
          g->setBalance(0);
        }
        n->setBalance(0);
      }
      return;
    }
  }
  else if (g->getRight() == p) {
    g->updateBalance(1);
    if(g->getBalance() == 0) {
      return;
    }
    else if (g->getBalance() == 1) {
      insertFix(p,g);
    }
    else if (g->getBalance() == 2) {
      if (n == p->getRight()) {
        rotateLeft(g);
        p->setBalance(0);
        g->setBalance(0);
      }
      else if (n == p->getLeft()) {
        rotateRight(p);
        rotateLeft(g);
        if(n->getBalance() == 1) {
          p->setBalance(0);
          g->setBalance(-1);
        }
        else if (n->getBalance() == 0) {
          p->setBalance(0);
          g->setBalance(0);
        }
        else if (n->getBalance() == -1) {
          p->setBalance(1);
          g->setBalance(0);
        }
        n->setBalance(0);
      }
      return;
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* n) {
  AVLNode<Key, Value>* c1 = n->getRight();
  AVLNode<Key, Value>* c2 = c1->getLeft();
  AVLNode<Key, Value>* p = n->getParent();
  c1->setParent(p);
  if (p == NULL) {
    this->root_ = c1;
  }
  else if (p->getLeft() == n) {
    p->setLeft(c1);
  }
  else if (p->getRight() == n) {
    p->setRight(c1);
  }
  n->setParent(c1);
  n->setRight(c2);
  c1->setLeft(n);
  if (c2 != NULL) {
    c2->setParent(n);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* n) {
  AVLNode<Key, Value>* c1 = n->getLeft();
  AVLNode<Key, Value>* c2 = c1->getRight();
  AVLNode<Key, Value>* p = n->getParent();
  c1->setParent(p);
  if (p == NULL) {
    this->root_ = c1;
  }
  else if (p->getLeft() == n) {
    p->setLeft(c1);
  }
  else if (p->getRight() == n) {
    p->setRight(c1);
  }
  n->setParent(c1);
  n->setLeft(c2);
  c1->setRight(n);
  if (c2 != NULL) {
    c2->setParent(n);
  }
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