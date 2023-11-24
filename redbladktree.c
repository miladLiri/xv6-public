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

// Function to print inorder traversal of the fixated tree
void inorder(struct node* trav)
{
    if (trav == NULL)
        return;
    inorder(trav->l);
    printf("%d ", trav->data->virtualtime);
    inorder(trav->r);
}

// Driver code
int main()
{
    int n = 7;
    int a[7] = { 7, 6, 5, 4, 3, 2, 1 };

    // Define and initialize the processes
    struct proc procs[7];
    procs[0].virtualtime = 7;
    procs[1].virtualtime = 6;
    procs[2].virtualtime = 5;
    procs[3].virtualtime = 4;
    procs[4].virtualtime = 3;
    procs[5].virtualtime = 2;
    procs[6].virtualtime = 1;

    for (int i = 0; i < n; i++) {
        struct node* temp = (struct node*)malloc(sizeof(struct node));
        temp->r = NULL;
        temp->l = NULL;
        temp->p = NULL;
        temp->data = &procs[i];
        temp->c = 1;

        root = bst(root, temp);
        fixup(root, temp);
        root->c = 0;
    }

    printf("Inorder Traversal of Created Tree:\n");
    inorder(root);

    return 0;
}