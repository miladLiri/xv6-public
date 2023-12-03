#include "spinlock.h"

struct RedBlackTree {
  int count;
  int rbTreeWeight;
  struct proc *root;
  struct proc *min_vRuntime;
  struct spinlock lock;
  int period;
}rbTree;

void treeInit(struct redblackTree *tree, char *lockName, int latency);

void insertProcess(struct RedBlackTree* tree, struct proc* p);

struct proc* retrieveProcess(struct RedBlackTree* tree, int latency, int min_granularity);