#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mytree.h"
#include "base.h"

namespace mytree {

MYTREE_T *constructor() {
    MYTREE_T *new_tree = typed_calloc(1, MYTREE_T);
    if (new_tree == nullptr) {
        return nullptr;
    }
    new_tree->size = 0;
    new_tree->root = nullptr;

    return new_tree;
}

function NODE_T *alloc_new_node(/*NODE_T *parent*/) {
    NODE_T *new_node = typed_calloc(1, NODE_T);
    if (new_node == nullptr) {
        return nullptr;
    }
    new_node->signature=signature;
    new_node->left = nullptr;
    new_node->right = nullptr;
    // new_node->parent = parent;

    return new_node;
}

void destructor(MYTREE_T *tree) {
    if (tree == nullptr) return;
    destructor(tree->root);
    free(tree);
}

void destructor(NODE_T *subtree) {
    if (subtree == nullptr) return;
    if (subtree->left != nullptr) {
        destructor(subtree->left);
    }
    if (subtree->right != nullptr) {
        destructor(subtree->right);
    }
    free(subtree);
}

bool verifier(NODE_T *subtree) {
    if (subtree == nullptr) return true;
    if (subtree->signature != signature) return false;
    return verifier(subtree->left) && verifier(subtree->right);
}

bool verifier(MYTREE_T *tree) {
    if (tree == nullptr) return false;
    return verifier(tree->root);
}

void print(MYTREE_T *tree) {
    verifier(tree) verified(return;);

    return print(tree->root);
}

void print(NODE_T *subtree) {
    verifier(subtree) verified(return;);

    printf("(");
    if (subtree->left != nullptr) print(subtree->left);
    printf("%d", atoi(subtree->data));
    if (subtree->right != nullptr) print(subtree->right);
    printf(")");
}

NODE_T *insert(MYTREE_T *tree, CONTAIRING_T value) {
    verifier(tree) verified(return nullptr;);
    if (value == nullptr) return nullptr;

    if (tree->root == nullptr) {
        NODE_T *new_node = alloc_new_node();
        if (new_node == nullptr) return nullptr;
        new_node->data = value;
        tree->root = new_node;
        tree->size = 1;
        return new_node;
    }

    NODE_T *res = insert(tree->root, value);
    if (res != nullptr) {
        tree->size += 1;
    }
    return res;
}

NODE_T *insert(NODE_T *subtree, CONTAIRING_T value) {
    verifier(subtree) verified(return nullptr;);
    if (value == nullptr) return nullptr;

    NODE_T *cur = subtree;
    while (true) {
        if (atoi(value) <= atoi(cur->data)) {
            if (cur->left == nullptr) {
                NODE_T *new_node = alloc_new_node();
                if (new_node == nullptr) return nullptr;
                new_node->data = value;
                cur->left = new_node;
                return new_node;
            } else {
                cur = cur->left;
            }
        } else {
            if (cur->right == nullptr) {
                NODE_T *new_node = alloc_new_node();
                if (new_node == nullptr) return nullptr;
                new_node->data = value;
                cur->right = new_node;
                return new_node;
            } else {
                cur = cur->right;
            }
        }
    }
}

}