#include <stdio.h>
#include <stdlib.h>
#include "proc.h"

typedef enum { RED, BLACK } Color;

typedef struct Node {
    struct proc* data;
    Color color;
    struct Node* parent;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct RBT {
    Node* root;
} RBT;

Node* createNode(struct proc* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->color = RED;
    newNode->parent = NULL;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

RBT* createRBT() {
    RBT* tree = (RBT*)malloc(sizeof(RBT));
    tree->root = NULL;
    return tree;
}

void leftRotate(RBT* tree, Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;
    y->parent = x->parent;

    if (x->parent == NULL)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void rightRotate(RBT* tree, Node* x) {
    Node* y = x->left;
    x->left = y->right;
    if (y->right != NULL)
        y->right->parent = x;
    y->parent = x->parent;

    if (x->parent == NULL)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->right = x;
    x->parent = y;
}

void fixInsertion(RBT* tree, Node* z) {
    while (z->parent != NULL && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            Node* y = z->parent->parent->right;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(tree, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(tree, z->parent->parent);
            }
        } else {
            Node* y = z->parent->parent->left;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(tree, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(tree, z->parent->parent);
            }
        }
    }

    tree->root->color = BLACK;
}

void insertRBT(RBT* tree, struct proc* data) {
    Node* z = createNode(data);
    Node* y = NULL;
    Node* x = tree->root;

    while (x != NULL) {
        y = x;
        if (z->data->virtualtime < x->data->virtualtime)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;
    if (y == NULL)
        tree->root = z;
    else if (z->data->virtualtime < y->data->virtualtime)
        y->left = z;
    else
        y->right = z;

    fixInsertion(tree, z);
}

void fixDeletion(RBT* tree, Node* x) {
    while (x != NULL && x != tree->root && x->color == BLACK) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;
            if (w != NULL && w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(tree, x->parent);
                w = x->parent->right;
            }

            if ((w != NULL && w->left != NULL && w->left->color == BLACK) &&
                (w != NULL && w->right != NULL && w->right->color == BLACK)) {
                if (w != NULL)
                    w->color = RED;
                x = x->parent;
            } else {
                if (w != NULL && w->right != NULL && w->right->color == BLACK) {
                    if (w->left != NULL)
                        w->left->color = BLACK;
                    if (w != NULL)
                        w->color = RED;
                    rightRotate(tree, w);
                    w = x->parent->right;
                }

                if (w != NULL)
                    w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w != NULL && w->right != NULL)
                    w->right->color = BLACK;
                leftRotate(tree, x->parent);
                x = tree->root;
            }
        } else {
            Node* w = x->parent->left;
            if (w != NULL && w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(tree, x->parent);
                w = x->parent->left;
            }

            if ((w != NULL && w->right != NULL && w->right->color == BLACK) &&
                (w != NULL && w->left != NULL && w->left->color == BLACK)) {
                if (w != NULL)
                    w->color = RED;
                x = x->parent;
            } else {
                if (w != NULL && w->left != NULL && w->left->color == BLACK) {
                    if (w->right != NULL)
                        w->right->color = BLACK;
                    if (w != NULL)
                        w->color = RED;
                    leftRotate(tree, w);
                    w = x->parent->left;
                }

                if (w != NULL)
                    w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w != NULL && w->left != NULL)
                    w->left->color = BLACK;
                rightRotate(tree, x->parent);
                x = tree->root;
            }
        }
    }

    if (x != NULL)
        x->color = BLACK;
}
Node* checkPID(Node* node,int pid){
    if(node == NULL){
        return NULL;
    }
    else if(node->data->pid == pid){
        return node;
    }    
    else{
        Node* left = checkPID(node->left, pid);
        Node* right = checkPID(node->right, pid);
        if(right == NULL && left == NULL){
            return NULL;
        }
        else if(right == NULL){
            return left;
        }
        else{
            return right;
        }
    }
    
}

void deleteRBT(RBT* tree, int pid) {
    Node* z = tree->root;
    Node* x;
    Node* y;

    z = checkPID(z, pid);
    
    if (z == NULL)
        return;

    if (z->left == NULL || z->right == NULL)
        y = z;
    else {
        Node* temp = z->right;
        while (temp->left != NULL)
            temp = temp->left;
        y = temp;
    }

    if (y->left != NULL)
        x = y->left;
    else
        x = y->right;

    if (x != NULL)
        x->parent = y->parent;

    if (y->parent == NULL)
        tree->root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;

    if (y != z)
        z->data = y->data;

    if (y->color == BLACK)
        fixDeletion(tree, x);

    free(y);
}

void printRBTInOrder(Node* node) {
    if (node == NULL)
        return;

    printRBTInOrder(node->left);
    printf("%d ", node->data);
    printRBTInOrder(node->right);
}
