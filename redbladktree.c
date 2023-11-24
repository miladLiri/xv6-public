#include <stdio.h>
#include <stdlib.h>
#include "proc.h"

// Structure to represent each node in a red-black tree
struct node {
    struct proc* data; // process
    int c;             // 1-red, 0-black
    struct node* p;    // parent
    struct node* r;    // right-child
    struct node* l;    // left child
};

struct node* root = NULL; // global root for the entire tree

// Function to perform BST insertion of a node
struct node* bst(struct node* trav, struct node* temp)
{
    // If the tree is empty, return a new node
    if (trav == NULL)
        return temp;

    // Otherwise recur down the tree
    if (temp->data->virtualtime < trav->data->virtualtime) {
        trav->l = bst(trav->l, temp);
        trav->l->p = trav;
    }
    else if (temp->data->virtualtime > trav->data->virtualtime) {
        trav->r = bst(trav->r, temp);
        trav->r->p = trav;
    }

    // Return the (unchanged) node pointer
    return trav;
}

// Function performing right rotation of the passed node
void rightrotate(struct node* temp)
{
    struct node* left = temp->l;
    temp->l = left->r;
    if (temp->l)
        temp->l->p = temp;
    left->p = temp->p;
    if (!temp->p)
        root = left;
    else if (temp == temp->p->l)
        temp->p->l = left;
    else
        temp->p->r = left;
    left->r = temp;
    temp->p = left;
}

// Function performing left rotation of the passed node
void leftrotate(struct node* temp)
{
    struct node* right = temp->r;
    temp->r = right->l;
    if (temp->r)
        temp->r->p = temp;
    right->p = temp->p;
    if (!temp->p)
        root = right;
    else if (temp == temp->p->l)
        temp->p->l = right;
    else
        temp->p->r = right;
    right->l = temp;
    temp->p = right;
}

// This function fixes violations caused by BST insertion
void fixup(struct node* root, struct node* pt)
{
    struct node* parent_pt = NULL;
    struct node* grand_parent_pt = NULL;

    while ((pt != root) && (pt->c != 0) && (pt->p->c == 1)) {
        parent_pt = pt->p;
        grand_parent_pt = pt->p->p;

        if (parent_pt == grand_parent_pt->l) {
            struct node* uncle_pt = grand_parent_pt->r;

            if (uncle_pt != NULL && uncle_pt->c == 1) {
                grand_parent_pt->c = 1;
                parent_pt->c = 0;
                uncle_pt->c = 0;
                pt = grand_parent_pt;
            }
            else {
                if (pt == parent_pt->r) {
                    leftrotate(parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->p;
                }

                rightrotate(grand_parent_pt);
                int t = parent_pt->c;
                parent_pt->c = grand_parent_pt->c;
                grand_parent_pt->c = t;
                pt = parent_pt;
            }
        }
        else {
            struct node* uncle_pt = grand_parent_pt->l;

            if ((uncle_pt != NULL) && (uncle_pt->c == 1)) {
                grand_parent_pt->c = 1;
                parent_pt->c = 0;
                uncle_pt->c = 0;
                pt = grand_parent_pt;
            }
            else {
                if (pt == parent_pt->l) {
                    rightrotate(parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->p;
                }

                leftrotate(grand_parent_pt);
                int t = parent_pt->c;
                parent_pt->c = grand_parent_pt->c;
                grand_parent_pt->c = t;
                pt = parent_pt;
            }
        }        
    }
}

// Function to find the minimum node in a given tree
struct node* minValueNode(struct node* node)
{
    struct node* current = node;

    // Find the leftmost leaf node
    while (current && current->l != NULL)
        current = current->l;

    return current;
}

// Function to replace a node with the child node
void replaceNode(struct node* node, struct node* child)
{
    if (node->p == NULL)
        root = child;
    else if (node == node->p->l)
        node->p->l = child;
    else
        node->p->r = child;

    if (child != NULL)
        child->p = node->p;
}

// Function to fix the violations caused by BST deletion
void fixDelete(struct node* node)
{
    struct node* sibling;
    while (node != root && node->c == 0) {
        if (node == node->p->l) {
            sibling = node->p->r;

            if (sibling->c == 1) {
                sibling->c = 0;
                node->p->c = 1;
                leftrotate(node->p);
                sibling = node->p->r;
            }

            if (sibling->l->c == 0 && sibling->r->c == 0) {
                sibling->c = 1;
                node = node->p;
            }
            else {
                if (sibling->r->c == 0) {
                    sibling->l->c = 0;
                    sibling->c = 1;
                    rightrotate(sibling);
                    sibling = node->p->r;
                }

                sibling->c = node->p->c;
                node->p->c = 0;
                sibling->r->c = 0;
                leftrotate(node->p);
                node = root;
            }
        }
        else {
            sibling = node->p->l;

            if (sibling->c == 1) {
                sibling->c = 0;
                node->p->c = 1;
                rightrotate(node->p);
                sibling = node->p->l;
            }

            if (sibling->r->c == 0 && sibling->l->c == 0) {
                sibling->c = 1;
                node = node->p;
            }
            else {
                if (sibling->l->c == 0) {
                    sibling->r->c = 0;
                    sibling->c = 1;
                    leftrotate(sibling);
                    sibling = node->p->l;
                }

                sibling->c = node->p->c;
                node->p->c = 0;
                sibling->l->c = 0;
                rightrotate(node->p);
                node = root;
            }
        }
    }
    node->c = 0;
}

// Function to delete a node from the red-black tree
struct node* deleteNode(struct node* root, int key)
{
    struct node* node = root;
    struct node* parent = NULL;
    struct node* toDelete = NULL;
    int found = 0;

    while (node != NULL && found == 0) {
        if (node->data->pid == key) {
            found = 1;
            toDelete = node;
        }

        if (found == 0) {
            if (node->data->pid < key)
                node = node->r;
            else
                node = node->l;
        }
    }

    if (found == 0) {        
        return root;
    }

    int color = toDelete->c;
    struct node* child;
    if (toDelete->l == NULL) {
        child = toDelete->r;
        replaceNode(toDelete, toDelete->r);
    }
    else if (toDelete->r == NULL) {
        child = toDelete->l;
        replaceNode(toDelete, toDelete->l);
    }
    else {
        struct node* minNode = minValueNode(toDelete->r);
        color = minNode->c;
        child = minNode->r;
        if (minNode->p == toDelete)
            parent = minNode;
        else {
            replaceNode(minNode, minNode->r);
            minNode->r = toDelete->r;
            minNode->r->p = minNode;
        }
        replaceNode(toDelete, minNode);
        minNode->l = toDelete->l;
        minNode->l->p = minNode;
        minNode->c = toDelete->c;
    }

    if (color == 0)
        fixDelete(child);

    return root;
}
