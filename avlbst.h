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
    AVLNode<Key, Value>* node_to_remove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (node_to_remove == nullptr) {
        return; 
    }

    if (node_to_remove->getLeft() != nullptr && node_to_remove->getRight() != nullptr) {
        AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key, Value>*>(this->predecessor(node_to_remove));
        nodeSwap(node_to_remove, predecessor);
    }

    AVLNode<Key, Value>* child_node = (node_to_remove->getRight() != nullptr) ? node_to_remove->getRight() : node_to_remove->getLeft();

    if (child_node != nullptr) {
        child_node->setParent(node_to_remove->getParent());
    }

    AVLNode<Key, Value>* parent_node = node_to_remove->getParent();
    int diff = 0; 
    if (parent_node == nullptr) {
        this->root_ = child_node;
    } else {
        if (node_to_remove == parent_node->getLeft()) {
            parent_node->setLeft(child_node);
            diff = 1;
        } else {
            parent_node->setRight(child_node);
            diff = -1;
        }
    }

    delete node_to_remove;

    removeFix(parent_node, diff);
}


template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff) {
  if (node == NULL) {
    return;
  }
  AVLNode<Key, Value>* parentNode = node->getParent();
  int nodeDiff = -1;
  node==parentNode->getLeft()
  if(parentNode != NULL && node) {
    nodeDiff = 1;
  }

  if(diff == -1) {
    int val = node->getBalance() + diff;
    if( val == -2) {
      AVLNode<Key, Value>* newParentNode = node->getLeft();
      if(newParentNode->getBalance() == -1) {
        rotateRight(node);
        node->setBalance(0);
        newParentNode->setBalance(0);
        removeFix(parentNode, nodeDiff);
      }
      else if (newParentNode->getBalance() == 0) {
        rotateRight(node);
        node->setBalance(-1);
        newParentNode->setBalance(1);
        return;
      }
      else if (newParentNode->getBalance() == 1){
        AVLNode<Key, Value>* temp = newParentNode->getRight();
        rotateLeft(newParentNode);
        rotateRight(node);
        int val2 = temp->getBalance();
        if(val2 == 1) {
          node->setBalance(0);
          newParentNode->setBalance(-1);
        }
        else if (val2 == 0) {
          node->setBalance(0);
          newParentNode->setBalance(0);
        }
        else if (val2 == -1) {
          node->setBalance(1);
          newParentNode->setBalance(0);
        }
        temp->setBalance(0);
        removeFix(parentNode, nodeDiff);
      }
    }
    else if (val == -1) {
      node->setBalance(-1);
      return;
    }
    else if (val == 0) {
      node->setBalance(0);
      removeFix(parentNode, nodeDiff);
    }
  }
  else if (diff == 1) {
    if (node->getBalance() + diff == 2) {
      AVLNode<Key, Value>* newP = node->getRight();
      if(newP->getBalance() == 1) {
        rotateLeft(node);
        node->setBalance(0);
        newP->setBalance(0);
        removeFix(parentNode, nodeDiff);
      }
      else if (newP->getBalance() == 0) {
        rotateLeft(node);
        node->setBalance(1);
        newP->setBalance(-1);
        return;
      }
      else if (newP->getBalance() == -1) {
        AVLNode<Key, Value>* temp = newP->getLeft();
        rotateRight(newP);
        rotateLeft(node);
        if(temp->getBalance() == -1) {
          node->setBalance(0);
          newP->setBalance(1);
        }
        else if (temp->getBalance() == 0) {
          node->setBalance(0);
          newP->setBalance(0);
        }
        else if (temp->getBalance() == 1) {
          node->setBalance(-1);
          newP->setBalance(0);
        }
        temp->setBalance(0);
        removeFix(parentNode, nodeDiff);
      }
    }
    else if(node->getBalance() + diff == 1) {
      node->setBalance(1);
      return;
    }
    else if (node->getBalance() + diff == 0) {
      node->setBalance(0);
      removeFix(parentNode, nodeDiff);
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* node, AVLNode<Key, Value>* parentNode)
{
  if(parentNode == NULL || parentNode->getParent() == NULL) {
    return;
  }
  AVLNode<Key, Value>* grand_parentNode = parentNode->getParent();
  if(grand_parentNode->getLeft() == parentNode) {
    grand_parentNode->updateBalance(-1);
    if(grand_parentNode->getBalance() == 0) return;
    else if (grand_parentNode->getBalance() == -1) {
      insertFix(parentNode, grand_parentNode);
    }
    else if (grand_parentNode->getBalance() == -2) {
      if(node == parentNode->getLeft()) {
        rotateRight(grand_parentNode);
        parentNode->setBalance(0);
        grand_parentNode->setBalance(0);
      }
      else if (node == parentNode->getRight()) {
        rotateLeft(parentNode);
        rotateRight(grand_parentNode);
        if(node->getBalance() == -1) {
          parentNode->setBalance(0);
          grand_parentNode->setBalance(1);
        }
        else if (node->getBalance() == 0) {
          parentNode->setBalance(0);
          grand_parentNode->setBalance(0);
        }
        else if (node->getBalance() == 1) {
          parentNode->setBalance(-1);
          grand_parentNode->setBalance(0);
        }
        node->setBalance(0);
      }
      return;
    }
  }
  else if (grand_parentNode->getRight() == parentNode) {
    grand_parentNode->updateBalance(1);
    if(grand_parentNode->getBalance() == 0) {
      return;
    }
    else if (grand_parentNode->getBalance() == 1) {
      insertFix(parentNode,grand_parentNode);
    }
    else if (grand_parentNode->getBalance() == 2) {
      if (node == parentNode->getRight()) {
        rotateLeft(grand_parentNode);
        parentNode->setBalance(0);
        grand_parentNode->setBalance(0);
      }
      else if (node == parentNode->getLeft()) {
        rotateRight(parentNode);
        rotateLeft(grand_parentNode);
        if(node->getBalance() == 1) {
          parentNode->setBalance(0);
          grand_parentNode->setBalance(-1);
        }
        else if (node->getBalance() == 0) {
          parentNode->setBalance(0);
          grand_parentNode->setBalance(0);
        }
        else if (node->getBalance() == -1) {
          parentNode->setBalance(1);
          grand_parentNode->setBalance(0);
        }
        node->setBalance(0);
      }
      return;
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
  AVLNode<Key, Value>* c1 = node->getRight();
  AVLNode<Key, Value>* c2 = c1->getLeft();
  AVLNode<Key, Value>* parentNode = node->getParent();
  c1->setParent(parentNode);
  if (parentNode == NULL) {
    this->root_ = c1;
  }
  else if (parentNode->getLeft() == node) {
    parentNode->setLeft(c1);
  }
  else if (parentNode->getRight() == node) {
    parentNode->setRight(c1);
  }
  node->setParent(c1);
  node->setRight(c2);
  c1->setLeft(node);
  if (c2 != NULL) {
    c2->setParent(node);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
  AVLNode<Key, Value>* c1 = node->getLeft();
  AVLNode<Key, Value>* c2 = c1->getRight();
  AVLNode<Key, Value>* parentNode = node->getParent();
  c1->setParent(parentNode);
  if (parentNode == NULL) {
    this->root_ = c1;
  }
  else if (parentNode->getLeft() == node) {
    parentNode->setLeft(c1);
  }
  else if (parentNode->getRight() == node) {
    parentNode->setRight(c1);
  }
  node->setParent(c1);
  node->setLeft(c2);
  c1->setRight(node);
  if (c2 != NULL) {
    c2->setParent(node);
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