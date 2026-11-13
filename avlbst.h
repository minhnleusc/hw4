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
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rebalance(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int diff);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }
    AVLNode<Key, Value>* current =
        static_cast<AVLNode<Key, Value>*>(this->root_);
    while(true) {
        if(new_item.first < current->getKey()) {
            if(current->getLeft() != NULL) {
                current = current->getLeft();
            }
            else {
                AVLNode<Key, Value>* node = new AVLNode<Key, Value>(
                    new_item.first, new_item.second, current);
                current->setLeft(node);
                insertFix(node);
                return;
            }
        }
        else if(current->getKey() < new_item.first) {
            if(current->getRight() != NULL) {
                current = current->getRight();
            }
            else {
                AVLNode<Key, Value>* node = new AVLNode<Key, Value>(
                    new_item.first, new_item.second, current);
                current->setRight(node);
                insertFix(node);
                return;
            }
        }
        else {
            current->setValue(new_item.second);
            return;
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
    AVLNode<Key, Value>* node =
        static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(node == NULL) {
        return;
    }
    if(node->getLeft() != NULL && node->getRight() != NULL) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(
            this->predecessor(node));
        nodeSwap(node, pred);
    }

    AVLNode<Key, Value>* child = node->getLeft();
    if(child == NULL) {
        child = node->getRight();
    }
    AVLNode<Key, Value>* parent = node->getParent();
    int diff = 0;
    if(parent != NULL) {
        // Losing left height raises balance; losing right height lowers it.
        diff = (parent->getLeft() == node) ? 1 : -1;
    }
    if(child != NULL) {
        child->setParent(parent);
    }
    if(parent == NULL) {
        this->root_ = child;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }
    delete node;
    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* top = node->getRight();
    AVLNode<Key, Value>* middle = top->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    top->setParent(parent);
    if(parent == NULL) {
        this->root_ = top;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(top);
    }
    else {
        parent->setRight(top);
    }
    node->setRight(middle);
    if(middle != NULL) {
        middle->setParent(node);
    }
    top->setLeft(node);
    node->setParent(top);

    int topBalance = top->getBalance();
    int nodeBalance = node->getBalance() - 1 -
                      (topBalance > 0 ? topBalance : 0);
    node->setBalance(static_cast<int8_t>(nodeBalance));
    top->setBalance(static_cast<int8_t>(
        topBalance - 1 + (nodeBalance < 0 ? nodeBalance : 0)));
    return top;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* top = node->getLeft();
    AVLNode<Key, Value>* middle = top->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    top->setParent(parent);
    if(parent == NULL) {
        this->root_ = top;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(top);
    }
    else {
        parent->setRight(top);
    }
    node->setLeft(middle);
    if(middle != NULL) {
        middle->setParent(node);
    }
    top->setRight(node);
    node->setParent(top);

    int topBalance = top->getBalance();
    int nodeBalance = node->getBalance() + 1 -
                      (topBalance < 0 ? topBalance : 0);
    node->setBalance(static_cast<int8_t>(nodeBalance));
    top->setBalance(static_cast<int8_t>(
        topBalance + 1 + (nodeBalance > 0 ? nodeBalance : 0)));
    return top;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node)
{
    if(node->getBalance() == -2) {
        if(node->getLeft()->getBalance() > 0) {
            rotateLeft(node->getLeft());
        }
        return rotateRight(node);
    }
    if(node->getBalance() == 2) {
        if(node->getRight()->getBalance() < 0) {
            rotateRight(node->getRight());
        }
        return rotateLeft(node);
    }
    return node;
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* parent = node->getParent();
    while(parent != NULL) {
        parent->updateBalance(parent->getLeft() == node ? -1 : 1);
        if(parent->getBalance() == 0) {
            return;
        }
        if(parent->getBalance() == -2 || parent->getBalance() == 2) {
            rebalance(parent);
            return;
        }
        node = parent;
        parent = parent->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
    while(node != NULL) {
        node->updateBalance(static_cast<int8_t>(diff));
        if(node->getBalance() == -1 || node->getBalance() == 1) {
            return;
        }

        AVLNode<Key, Value>* top = rebalance(node);
        // A nonzero result after a deletion rotation means height is unchanged.
        if(top->getBalance() != 0) {
            return;
        }
        AVLNode<Key, Value>* parent = top->getParent();
        if(parent == NULL) {
            return;
        }
        diff = (parent->getLeft() == top) ? 1 : -1;
        node = parent;
    }
}


#endif
