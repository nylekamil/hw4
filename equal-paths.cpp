#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool equalPathsHelp(Node* root, int depth, int& leafDepth, bool& foundFirstLeaf); 

//helper function
bool equalPathsHelp(Node* root, int depth, int& leafDepth, bool& foundFirstLeaf) {
    if (root == nullptr) { //checks if empty subtree  
        return true; 
    }
    //check to see if leaf (no left and right children)
    if (root->left == nullptr && root->right == nullptr) {
        //see if this is the first leaf 
        if (!foundFirstLeaf) {
            foundFirstLeaf = true; //update foundFirstLeaf
            leafDepth = depth; //hold depth of first leaf
            return true; 
        } else { //for non first leaf
            if (depth == leafDepth) {
                return true; 
            } else {
                return false; 
            }
        }
    }
    //need to go down both left and right side 
    bool isLeftValid = equalPathsHelp(root->left, depth+1, leafDepth, foundFirstLeaf); 
    bool isRightValid = equalPathsHelp(root->right, depth+1, leafDepth, foundFirstLeaf); 
    if (isLeftValid == true && isRightValid == true) { //checks to see if both are true
        return true; 
    } else {
        return false; 
    }
}

bool equalPaths(Node * root)
{
    int leafDepth = 0; //intialize leafDepth 
    bool foundFirstLeaf = false; //intialize foundFirstLeaf
    return equalPathsHelp(root, 0, leafDepth, foundFirstLeaf); 
}

