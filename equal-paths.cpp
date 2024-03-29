#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkEqualPaths(Node* node, int& depth, int currentDepth);

bool equalPaths(Node * root)
{
    if (!root) return true; 

    int depth = -1;
    return checkEqualPaths(root, depth, 0);
}

bool checkEqualPaths(Node* node, int& depth, int currentDepth) {
    if (!node) {
        if (depth == -1) {
            depth = currentDepth;
            return true;
        }
        return depth == currentDepth;
    }
    return checkEqualPaths(node->left, depth, currentDepth + 1) && 
           checkEqualPaths(node->right, depth, currentDepth + 1);
}