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


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_== NULL) {
      AVLNode<Key, Value>* newRoot = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
      newRoot->setBalance(0);
      newRoot->setLeft(NULL);
      newRoot->setRight(NULL);
      this->root_= newRoot;
      return;
    }
    else if(this->internalFind(new_item.first) != NULL) {
      this->internalFind(new_item.first)->setValue(new_item.second);
      return;
    }
    else if (this->internalFind(new_item.first) == NULL) {
      AVLNode<Key, Value>* avlRoot = static_cast<AVLNode<Key, Value>*>(this->root_);
      while (avlRoot != NULL) {
        if(avlRoot->getKey() > new_item.first) {
          AVLNode<Key, Value>* temp = new AVLNode<Key, Value>(new_item.first, new_item.second, avlRoot);
          if(avlRoot->getLeft() != NULL) {
            avlRoot = avlRoot->getLeft();
          }
          else {
            avlRoot->setLeft(temp);
            temp->setParent(avlRoot);
            temp->setBalance(0);
            if(avlRoot->getBalance() == 1 || avlRoot->getBalance() == -1) {
              avlRoot->setBalance(0);
            }
            else if (avlRoot->getBalance() == 0) {
              avlRoot->updateBalance(-1);
              insertFix(temp, avlRoot);
            }
            return;
          }
        }
        else if (avlRoot->getKey() < new_item.first) {
          AVLNode<Key, Value>* temp = new AVLNode<Key, Value>(new_item.first, new_item.second, avlRoot);
          if(avlRoot->getRight() != NULL) {
            avlRoot = avlRoot->getRight();
          }
          else {
            avlRoot->setRight(temp);
            temp->setParent(avlRoot);
            temp->setBalance(0);
            if(avlRoot->getBalance() == 1 || avlRoot->getBalance() == -1) {
              avlRoot->setBalance(0);
            }
            else if (avlRoot->getBalance() == 0) {
              avlRoot->updateBalance(1);
              insertFix(temp, temp->getParent());
            }
            return;
          }
        }
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
    // TODO
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (temp == NULL) {
      return;
    }
    if(temp->getLeft() != NULL && temp->getRight() != NULL) {
      AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(temp));
      nodeSwap(temp, pred); 
    }
    AVLNode<Key, Value> * child = temp->getLeft();
    if(temp->getRight() != NULL) {
      child = temp->getRight();
    }
    AVLNode<Key, Value>* parent = temp->getParent();
    if(child != NULL) {
      child->setParent(parent);
    }
    int diff;
    if(parent == NULL) this->root_ = child;
    else {
      if(temp == parent->getLeft()) {
        parent->setLeft(child);
        diff = 1;
      }
      else {
        parent->setRight(child);
        diff = -1;
      }
    }
    delete temp;
    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff) {
  if (n == NULL) {
    return;
  }
  AVLNode<Key, Value>* p = n->getParent();
  int ndiff = -1;
  if(p != NULL && n==p->getLeft()) {
    ndiff = 1;
  }
  if(diff == -1) {
    if(n->getBalance() + diff == -2) {
      AVLNode<Key, Value>* newP = n->getLeft();
      if(newP->getBalance() == -1) {
        rotateRight(n);
        n->setBalance(0);
        newP->setBalance(0);
        removeFix(p, ndiff);
      }
      else if (newP->getBalance() == 0) {
        rotateRight(n);
        n->setBalance(-1);
        newP->setBalance(1);
        return;
      }
      else if (newP->getBalance() == 1){
        AVLNode<Key, Value>* temp = newP->getRight();
        rotateLeft(newP);
        rotateRight(n);
        if(temp->getBalance() == 1) {
          n->setBalance(0);
          newP->setBalance(-1);
        }
        else if (temp->getBalance() == 0) {
          n->setBalance(0);
          newP->setBalance(0);
        }
        else if (temp->getBalance() == -1) {
          n->setBalance(1);
          newP->setBalance(0);
        }
        temp->setBalance(0);
        removeFix(p, ndiff);
      }
    }
    else if (n->getBalance() + diff == -1) {
      n->setBalance(-1);
      return;
    }
    else if (n->getBalance() + diff == 0) {
      n->setBalance(0);
      removeFix(p, ndiff);
    }
  }
  else if (diff == 1) {
    if (n->getBalance() + diff == 2) {
      AVLNode<Key, Value>* newP = n->getRight();
      if(newP->getBalance() == 1) {
        rotateLeft(n);
        n->setBalance(0);
        newP->setBalance(0);
        removeFix(p, ndiff);
      }
      else if (newP->getBalance() == 0) {
        rotateLeft(n);
        n->setBalance(1);
        newP->setBalance(-1);
        return;
      }
      else if (newP->getBalance() == -1) {
        AVLNode<Key, Value>* temp = newP->getLeft();
        rotateRight(newP);
        rotateLeft(n);
        if(temp->getBalance() == -1) {
          n->setBalance(0);
          newP->setBalance(1);
        }
        else if (temp->getBalance() == 0) {
          n->setBalance(0);
          newP->setBalance(0);
        }
        else if (temp->getBalance() == 1) {
          n->setBalance(-1);
          newP->setBalance(0);
        }
        temp->setBalance(0);
        removeFix(p, ndiff);
      }
    }
    else if(n->getBalance() + diff == 1) {
      n->setBalance(1);
      return;
    }
    else if (n->getBalance() + diff == 0) {
      n->setBalance(0);
      removeFix(p, ndiff);
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