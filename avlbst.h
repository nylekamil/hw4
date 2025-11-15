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
    //one rotation (left and right)
    void roLeft(AVLNode<Key,Value>* node); 
    void roRight(AVLNode<Key,Value>* node); 
    //rotate after insert (balance)
    void afterInsert(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node); 
    //rotate after remove (balance)
    void afterRemove(AVLNode<Key, Value>* node, int8_t diff); //diff is +1 when left subtree shorter, -1 if right subtree shorter
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    //first, need to look when tree is empty (becomes root)
    if (this->root_ == NULL) {
        //make new node with no parent
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL); 
        return;
    }
    //start at root 
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_); //current (need static)
    AVLNode<Key, Value>* parent = NULL; //parent
    //now look for key or match 
    while(current != NULL) {
        parent = current; 
        //new key in left tree
        if (new_item.first < current->getKey()) {
            current = current->getLeft(); 
        } //new key in right tree
        else if (new_item.first > current->getKey()) {
            current = current->getRight(); 
        } 
        else {
            //key already exists 
            current->setValue(new_item.second);
            return;
        }
    }
    //make new node (put it under parent)
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    //decide wether to insert at left or right 
    //left 
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode); //insert
        parent->updateBalance(-1); //updateBalance
    } 
    else { //right
        parent->setRight(newNode); //insert
        parent->updateBalance(1); //updateBalance
    }
    if (parent->getBalance() != 0) {
        afterInsert(parent, newNode); //afterInsert
    }
    
}

//roLeft
template<class Key, class Value>
void AVLTree<Key, Value>::roLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* rightChild = node->getRight(); //right child
    int8_t childBalance = rightChild->getBalance(); 

    node->setRight(rightChild->getLeft()); //makes node have the right child's left subtree as new right child
    //see if right child had a left subtree 
    if (rightChild->getLeft() != NULL) {
        rightChild->getLeft()->setParent(node); //update parent
    }
    //make right child connect to nodes orig. parent
    rightChild->setParent(node->getParent()); 
    //check if node was root 
    if (node->getParent() == NULL) {
        this->root_ = rightChild; //right child is now root 
    }
    else if (node == node->getParent()->getLeft()){
        node->getParent()->setLeft(rightChild); //put right child in nodes place
    } 
    else {
        node->getParent()->setRight(rightChild); 
    }
    //now make node the left child (of rightChild)
    rightChild->setLeft(node);
    node->setParent(rightChild);   
    
    //balance 
    if (childBalance == 0) {
        node->setBalance(1);
        rightChild->setBalance(-1);
    }
    else {
        node->setBalance(0);
        rightChild->setBalance(0);
    }

}

//roRight 
template<class Key, class Value>
void AVLTree<Key, Value>::roRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* leftChild = node->getLeft(); //left child
    int8_t childBalance = leftChild -> getBalance(); 

    node->setLeft(leftChild->getRight()); //makes node have the left child's right subtree as new left child
    //see if left child had a right subtree 
    if (leftChild->getRight() != NULL) {
        leftChild->getRight()->setParent(node); //update parent
    }
    //make left child connect to nodes orig. parent
    leftChild->setParent(node->getParent()); 
    //check if node was root 
    if (node->getParent() == NULL) {
        this->root_ = leftChild; //left child is now root 
    }
    else if (node == node->getParent()->getRight()){
        node->getParent()->setRight(leftChild); //put left child in nodes place
    } 
    else {
        node->getParent()->setLeft(leftChild); 
    }
    //now make node the right child (of leftChild)
    leftChild->setRight(node);
    node->setParent(leftChild); 

    //balances
    if (childBalance == 0) {
        node->setBalance(-1);
        leftChild->setBalance(1);
    } else {
        node->setBalance(0);
        leftChild->setBalance(0);
    }
}

//afterInsert
template<class Key, class Value>
void AVLTree<Key, Value>::afterInsert(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node)
{
    //check if there is a parent 
    if (parent == NULL || parent->getParent() == NULL) {
        return; 
    }
    //store grandparent (of new node that was inserted)
    AVLNode<Key, Value>* grandparent = parent -> getParent(); 
    //check to see if there is no grandparent 
    if (grandparent == NULL) {
        return;
    } 
    //first, need to see if parent is left child of grandparent 
    if (parent == grandparent->getLeft()) {
        grandparent->updateBalance(-1); //(left taller)
        //check to see if balanced 
        if (grandparent->getBalance() == 0) {
            return;
        }
        else if (grandparent->getBalance() == -1) { 
            afterInsert(grandparent, parent); //continue
            return;
        }
        else if (grandparent->getBalance() == -2) { //has to be rotated
            //have to see if either left or right
            if (node == parent->getLeft()) {
                roRight(grandparent); 
                parent->setBalance(0); //balance 
                grandparent->setBalance(0); //balance
                return;
            } else { //LEFT-RIGHT
                int8_t childBalance = node->getBalance(); 
                roLeft(parent);
                roRight(grandparent);
                if (childBalance == -1){
                    grandparent->setBalance(1);
                    parent->setBalance(0);
                } else if (childBalance == 0) {
                    grandparent->setBalance(0);
                    parent->setBalance(0);
                }
                else {
                    grandparent->setBalance(0);
                    parent->setBalance(-1);
                }
                node->setBalance(0);
                return; 
            }
        }
     } else { //parent is right child
        //insert happened in right subtree
        grandparent->updateBalance(1); //right taller
        //check to see if grandparent is balanced
        if (grandparent->getBalance() == 0) {
            return;
        }
        else if (grandparent->getBalance() == 1) {
            afterInsert(grandparent, parent); //continue 
            return;
        }
        else if (grandparent->getBalance() == 2) { //right side more
            //check if right-right or right-left 
            if (node == parent->getRight()) {
                roLeft(grandparent);
                parent->setBalance(0); //balance
                grandparent->setBalance(0); 
                return;
            } 
            else { //RIGHT - LEFT
                int8_t childBalance = node->getBalance(); 
                roRight(parent);
                roLeft(grandparent);
                if (childBalance == 1){
                    grandparent->setBalance(-1);
                    parent->setBalance(0);
                } else if (childBalance == 0) {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                }
                else {
                    grandparent->setBalance(0);
                    parent->setBalance(1);
                }
                node->setBalance(0);
                return;
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
    AVLNode<Key, Value>* removeNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(key)); //find node (static)
    //check if key is not in tree 
    if (removeNode == NULL) {
        return;
    }
    //if 2 children --> swap w/ pred. 
    if (removeNode->getLeft() != NULL && removeNode->getRight() != NULL) {
        //greatest in left subtree
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(removeNode)); 
        //swap 
        nodeSwap(removeNode, pred); 
        //removeNode has at most 1 child
    }
    AVLNode<Key, Value>* child = NULL; 
    //if left exists, go with that, otherwise go with right 
    if (removeNode->getLeft() != NULL) {
        child = removeNode->getLeft(); 
    }
    else {
        child = removeNode->getRight(); 
    }
    AVLNode<Key, Value>* parent = removeNode->getParent();
    //if child is there, connect to removeNode parent 
    if(child != NULL) {
        child->setParent(parent); 
    }
    //+1 -> left subtree got smaller
    //-1 -> right subtree got smaller 
    int8_t diff = 0; 
    //if removing root then update root
    if (parent == NULL){
        this->root_ = child; 
    }
    else if (parent->getLeft() == removeNode) {
        //removeNode --> left child then left subtree of parent gets smaller
        parent->setLeft(child);
        diff = 1; //+1
    }
    else {
        //removeNode --> right child then right subtree of parent get smaller
        parent->setRight(child);
        diff = -1; //-1
    }
    delete removeNode; 
    //balance again 
    if (parent != NULL) {
        afterRemove(parent, diff); 
    }
}

//afterRemove
template<class Key, class Value>
void AVLTree<Key, Value>::afterRemove(AVLNode<Key,Value>* node, int8_t diff)
{
    if (node == NULL) {
        return;
    }
    AVLNode<Key, Value>* parent = node->getParent(); //parent
    int8_t ndiff = 0; //which side of parent node is  
    if (parent != NULL) {
        if (node == parent->getLeft()) {
            ndiff = 1; 
        } else {
            ndiff = -1; 
        }
    }

    //store beforeBalance and afterBalance of node 
    int8_t beforeBalance = node->getBalance(); 
    int8_t afterBalance = beforeBalance + diff; //(after side got smaller)
    //update balance 
    node->setBalance(afterBalance); 

    //first, look at when node is unbalanced by 2 (left)
    if (afterBalance == -2) {
        //left child 
        AVLNode<Key, Value>* leftChild = node->getLeft(); 
        int8_t leftBalance = leftChild->getBalance(); 
    
        //within this, look for left-left or left-0 
        if (leftBalance <= 0) {
            //one right rotation 
            roRight(node);
            //if left was balanced (before rotation)
            if (leftBalance == 0) {
                node->setBalance(-1);
                leftChild->setBalance(1); 
                return; 
            }
            else {
                //left child --> balance -1 
                node->setBalance(0);
                leftChild->setBalance(0); 
                afterRemove(parent, ndiff); //continue
            }
        }
        //next, within this, look for leftBalance more than 0 
        else {
            AVLNode<Key, Value>* grandchild = leftChild->getRight(); 
            int8_t grandBalance = grandchild -> getBalance(); 
        
        //double rotate 
        roLeft(leftChild);
        roRight(node); 

        //change balances
        if (grandBalance == -1) {
            //too left
            node->setBalance(1);
            leftChild->setBalance(0);
        }
        else if (grandBalance == 0) {
            //grandchild balanced
            node->setBalance(0);
            leftChild->setBalance(0);
        } 
        else {
            //too right
            node->setBalance(0);
            leftChild->setBalance(-1); 
        }
        grandchild->setBalance(0); //balanced
        afterRemove(parent, ndiff); //continue
        }
        return;
    }


    //next, need to look at node when right unbalanced
    else if (afterBalance == 2) {
        //right child 
        AVLNode<Key, Value>* rightChild = node->getRight();
        int8_t rightBalance = rightChild->getBalance(); 
        //within this need to see if right-right or right-0 
        if (rightBalance >= 0) {
            roLeft(node);
            if (rightBalance == 0){ //already balanced 
                node->setBalance(1);
                rightChild->setBalance(-1);
                return;
            }
            else {
                node->setBalance(0);
                rightChild->setBalance(0);
                afterRemove(parent, ndiff); //continue
            }
        }
        else {
            //when rightBalance < 0
            AVLNode<Key, Value>* grandchild = rightChild->getLeft();
            int8_t grandBalance = grandchild->getBalance(); 
            roRight(rightChild);
            roLeft(node); 

            //change based on grandchild balance 
            if (grandBalance == 1){
                //right 
                node->setBalance(-1);
                rightChild->setBalance(0); 
            }
            else if (grandBalance == 0) { //already balanced
                node -> setBalance(0); 
                rightChild->setBalance(0);
            }
            else {
                //left 
                node->setBalance(0);
                rightChild->setBalance(1); 
            }
            grandchild->setBalance(0); //balance
            afterRemove(parent, ndiff); //continue
        }
        return;
    } 
    //when afterBalance is 0 
    else if (afterBalance == 0) {
        afterRemove(parent, ndiff); 
        return; 
    }
    //when afterBalance is +1 or -1 
    else {
        return;
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
