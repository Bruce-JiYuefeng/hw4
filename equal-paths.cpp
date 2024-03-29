#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkEqualPaths(Node* node, int& leafDepth, int currentDepth);

bool equalPaths(Node * root)
{
    if (!root) return true; 

    int leafDepth = -1; 
    return checkEqualPaths(root, leafDepth, 0);
}

bool checkEqualPaths(Node* node, int& leafDepth, int currentDepth) {
    if (!node) return true; 

    if (!node->left && !node->right) {
        if (leafDepth == -1) {
            leafDepth = currentDepth; 
            return true;
        }
        return leafDepth == currentDepth; 
    }

    return checkEqualPaths(node->left, leafDepth, currentDepth + 1) &&
           checkEqualPaths(node->right, leafDepth, currentDepth + 1);
}