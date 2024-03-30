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
    virtual void insert(const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* current);
    void rotateRight(AVLNode<Key, Value>* current);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
    void removeFix(AVLNode<Key, Value>* current, int8_t diff);
	  AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* current);

};


/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // TODO
    AVLNode<Key, Value>* temp = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    temp->setBalance(0);
    if (!this->root_) this->root_ = temp;
    else {
        AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
        bool check = true;
       
        while (curr) {
            if (temp->getKey() == curr->getKey()) {
                nodeSwap(temp, curr);
                delete curr;
                check = false;
                break;
            }
            else if (temp->getKey() < curr->getKey()) {
                if (!curr->getLeft()) {
                    curr->setLeft(temp);
                    temp->setParent(curr);
                    break;
                }
                curr = curr->getLeft();
            }        
            else {
                if (!curr->getRight()) {
                    curr->setRight(temp);
                    temp->setParent(curr);
                    break;
                }
                curr = curr->getRight();
            }
        }
    
        if (check) {
            AVLNode<Key, Value>* parent = temp->getParent();
            if (abs(parent->getBalance()) == 1) {
                parent->setBalance(0);
            }
            else {
                if (parent->getLeft() == temp) {
                    parent->updateBalance(-1);
                }
                else {
                    parent->updateBalance(1);
                }
                insertFix(parent, temp);
            }
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // TODO
    if (!this->root_) {
        return;
    }

    if (this->internalFind(key)) {
        AVLNode<Key, Value>* deleteNode = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));

        if (deleteNode->getLeft() && deleteNode->getRight()) {
            nodeSwap(deleteNode, predecessor(deleteNode));
        }

        AVLNode<Key, Value>* parent = deleteNode->getParent();
        int8_t diff = 0;
				if (parent){
						if (parent->getLeft() == deleteNode) {
								diff = 1;
						}
						else {
								diff = -1;
						}
				}

        if (deleteNode->getLeft()) {
            deleteNode->getLeft()->setParent(parent);

            if (deleteNode == this->root_) {
                this->root_ = deleteNode->getLeft();

                if (deleteNode->getRight()) {
                    this->root_->setRight(deleteNode->getRight());
                    deleteNode->getRight()->setParent(this->root_);
                }
            }
            else {
                if (parent->getLeft() == deleteNode) {
                    parent->setLeft(deleteNode->getLeft());
                }
                else {
                    parent->setRight(deleteNode->getLeft());
                }
            }
        }
        else if (deleteNode->getRight()) {
            deleteNode->getRight()->setParent(parent);

            if (deleteNode == this->root_) {
                this->root_ = deleteNode->getRight();

                if (deleteNode->getLeft()) {
                    this->root_->setLeft(deleteNode->getLeft());
                    deleteNode->getLeft()->setParent(this->root_);
                }
            }
            else {
                if (parent->getLeft() == deleteNode) {
                    parent->setLeft(deleteNode->getRight());
                }
                else {
                    parent->setRight(deleteNode->getRight());
                }
            }
        }
        else {
            if (deleteNode == this->root_) {
                this->root_ = nullptr;
            }
            else {
                if (parent->getLeft() == deleteNode) {
                    parent->setLeft(nullptr);
                }
                else {
                    parent->setRight(nullptr);
                }
            }
        }
        delete deleteNode;
        removeFix(parent, diff);
    }
}

// Helper functions for insert and remove
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* current) {
    AVLNode<Key, Value>* parent = current->getParent();
    AVLNode<Key, Value>* child = current->getRight();
    child->setParent(current->getParent());
    
    if (!parent) {
        this->root_ = child;
    }
    else {
        if (parent->getLeft() == current) {
            parent->setLeft(child);
        }
        else {
            parent->setRight(child);
        }
    }
    current->setParent(child);
    current->setRight(child->getLeft());
	if (child->getLeft()) {
		child->getLeft()->setParent(current);
	}

	child->setLeft(current);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* current) {
    AVLNode<Key, Value>* parent = current->getParent();
    AVLNode<Key, Value>* child = current->getLeft();
    child->setParent(current->getParent());
    
    if (!parent) {
        this->root_ = child;
    }
    else {
        if (parent->getLeft() == current) {
            parent->setLeft(child);
        }
        else {
            parent->setRight(child);
        }
    }
    current->setParent(child);
    current->setLeft(child->getRight());
	  if (child->getRight()) {
		    child->getRight()->setParent(current);
	  }
	  child->setRight(current);
}

// Refer to Slides
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child)
{
    AVLNode<Key, Value>* grandparent = parent->getParent();
    if (!parent || !grandparent) {
        return;
    }

    // left
    if (grandparent->getLeft() == parent) {
        grandparent->updateBalance(-1);

        if (grandparent->getBalance() == 0) {
            return;
        }
        else if (grandparent->getBalance() == -1) {
            insertFix(grandparent, parent);
        }
        else {
            if (parent->getLeft() == child) {
                rotateRight(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            }
            else {
                rotateLeft(parent);
                rotateRight(grandparent);

                if (child->getBalance() == -1) {
                    parent->setBalance(0);
                    grandparent->setBalance(1);
                    child->setBalance(0);
                }
                else if (child->getBalance() == 0) {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    child->setBalance(0);
                }
                else {
                    parent->setBalance(-1);
                    grandparent->setBalance(0);
                    child->setBalance(0);
                }
            }
        }
    }
    
    // right
    else if (grandparent->getRight() == parent) {
        grandparent->updateBalance(1);

        if (grandparent->getBalance() == 0) {
            return;
        }
        else if (grandparent->getBalance() == 1) {
            insertFix(grandparent, parent);
        }
        else {
            if (parent->getRight() == child) {
                rotateLeft(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            }
            else {
                rotateRight(parent);
                rotateLeft(grandparent);

                if (child->getBalance() == 0) {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    child->setBalance(0);
                }
                else if (child->getBalance() == 1) {
                    parent->setBalance(0);
                    grandparent->setBalance(-1);
                    child->setBalance(0);
                }
                else {
                    parent->setBalance(1);
                    grandparent->setBalance(0);
                    child->setBalance(0);
                }
            }
        }
    }
}

// Refer to Slides
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* current, int8_t diff)
{
    if (!current) {
        return;
    }

    AVLNode<Key, Value>* parent = current->getParent();
    int8_t nextDiff = 0;
    if (parent) {
        if (parent->getLeft() == current) {
            nextDiff = 1;
        }
        else if (parent->getRight() == current) {
            nextDiff = -1;
        }
    }

    if (diff == -1) {
        if (current->getBalance() + diff == -2) {
            AVLNode<Key, Value>* child = current->getLeft();

            if (child->getBalance() == -1) {
                rotateRight(current);
                current->setBalance(0);
                child->setBalance(0);
                removeFix(parent, nextDiff);
            }
            else if (child->getBalance() == 0) {
                rotateRight(current);
                current->setBalance(-1);
                child->setBalance(1);
            }
            else {
                AVLNode<Key, Value>* grandchild = child->getRight();
                rotateLeft(child);
                rotateRight(current);

                if (grandchild->getBalance() == 0) {
                    current->setBalance(0);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }
                else if (grandchild->getBalance() == 1) {
                    current->setBalance(0);
                    child->setBalance(-1);
                    grandchild->setBalance(0);
                }
                else {
                    current->setBalance(1);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }

                removeFix(parent, nextDiff);
            }
        }
        else if (current->getBalance() + diff == -1) {
            current->setBalance(-1);
        }
        else if (current->getBalance() + diff == 0) {
            current->setBalance(0);
            removeFix(parent, nextDiff);
        }
    }
    else if (diff == 1) {
        if (current->getBalance() + diff == 2) {
            AVLNode<Key, Value>* child = current->getRight();

            if (child->getBalance() == 0) {
                rotateLeft(current);
                current->setBalance(1);
                child->setBalance(-1);
            }
            else if (child->getBalance() == 1) {
                rotateLeft(current);
                current->setBalance(0);
                child->setBalance(0);
                removeFix(parent, nextDiff);
            }
            else {
                AVLNode<Key, Value>* grandchild = child->getLeft();
                rotateRight(child);
                rotateLeft(current);

                if (grandchild->getBalance() == -1) {
                    current->setBalance(0);
                    child->setBalance(1);
                    grandchild->setBalance(0);
                }
                else if (grandchild->getBalance() == 0) {
                    current->setBalance(0);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }
                else {
                    current->setBalance(-1);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }

                removeFix(parent, nextDiff);
            }
        }
        else if (current->getBalance() + diff == 1) {
            current->setBalance(1);
        }
        else if (current->getBalance() + diff == 0) {
            current->setBalance(0);
            removeFix(parent, nextDiff);
        }
    }
}


template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* current) {
    if (!current) return nullptr;

    // Go left once then right as far as possible.
    if (current->getLeft()) {
        AVLNode<Key, Value>* pred = current->getLeft();
        while (pred->getRight()) {
            pred = pred->getRight();
        }
        return pred;
    }

    // No left child, go up until you've come from a right child.
    AVLNode<Key, Value>* pred = current;
    while (pred->getParent() && pred->getParent()->getLeft() == pred) {
        pred = pred->getParent();
    }

    // Return the parent if from a right child, otherwise nullptr.
    return pred->getParent();
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