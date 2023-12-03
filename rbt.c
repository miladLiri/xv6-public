#include "proc.h"
#include "rbt.h"

////////Red Black Tree functions for operations of Insertion and retrieving, while maintaining Red Black Tree properties

/*
  rbinit(struct RedBlackTree*, char)
  parameters: pointer that contains the address of the red-black tree and a string containing the name of the lock
  returns: none
  This function will initialize the red black tree data structure.
*/
void
treeInit(struct RedBlackTree *tree, char *lockName, int latency)
{
  initlock(&tree->lock, lockName);
  tree->count = 0;
  tree->root = 0;
  tree->rbTreeWeight = 0;
  tree->min_vRuntime = 0;

  //Initially set time slice factor for all processes
  tree->period = latency;
}

/*
  calculateWeight(int)
  parameters: the process's niceValue
  returns: an integer that signifies the weight of the process the address points to.
  This function will calculate each individual process's weight in respect to it's nice value.

  //-Nice value can be in between -20 to 19 in the linux kernel, but for our xv6 implementation we will use the range 0 to 30
  The default nice value for a process is set to 0

  The formula to determine weight of process is:
  1024/(1.25 ^ nice value of process)
*/
int
calculateWeight(int nice){

  double denominator = 1.25;

  //In order to ensure correct utilization of process priority during the time slice calculation
  //If a process has a higher nice value given, then for the formula to accurately utlize the priority level without losing precision
  //due to fraction casted to an int, it will give it a default value that will represent the same priority level in the system.
  if(nice > 30){
	nice = 30;
  }
  
  //While loop to calculate (1.25 ^ nice value) for denominator of formula to find weight. 
  int iterator = 0;
  while (iterator < nice && nice > 0){
  	denominator = denominator * 1.25;
  }

  return (int) (1024/denominator);
}

/*
  emptyTree(struct RedBlackTree*)
  parameters: pointer that contains the address of the red-black tree structure
  returns: none
  This function will determine if the tree is empty or not, i.e the tree has no processes in it.
*/
int
emptyTree(struct RedBlackTree *tree)
{
  return tree->count == 0;
}

/*
  fullTree(struct RedBlackTree*)
  parameters: pointer that contains the address of the red-black tree structure
  returns: none
  This function will determine if the tree is full, i.e the maximum alloted number of processes in the system
*/
int
fullTree(struct RedBlackTree *tree)
{
  return tree->count == NPROC;
}

//This two process retrieval functions will retrive the grandparent or uncle process of the process passed into the functions. This is done to preserve red black tree properties by altering states and positions of the tree.
struct proc*
retrieveGrandparentproc(struct proc* process){
  if(process != 0 && process->parentP != 0){
	return process->parentP->parentP;
  } 
	
  return 0;
}

struct proc*
retrieveUncleproc(struct proc* process){
  struct proc* grandParent = retrieveGrandparentproc(process);
  if(grandParent != 0){
	if(process->parentP == grandParent->left){
		return grandParent->right;
	} else {
		return grandParent->left;
	}
  }
	
  return 0;
}


/*
  rotateLeft(struct RedBlackTree*, struct proc*)
  parameters:The red black tree pointer to access and modify the root, and the current process in the tree that will be rotated to the left
  returns:none
  This function will perform a rotation on the process structure in the tree that is passed into the function. 
  It will perform a left rotation, where it will move down leftward in the tree and its right process will be moved up to its place.
*/
void 
rotateLeft(struct RedBlackTree* tree, struct proc* positionProc){
  struct proc* save_right_Proc = positionProc->right;
	
  positionProc->right = save_right_Proc->left;
  if(save_right_Proc->left != 0)
	save_right_Proc->left->parentP = positionProc;
  save_right_Proc->parentP = positionProc->parentP;
	
  if(positionProc->parentP == 0){
	tree->root = save_right_Proc;
  } else if(positionProc == positionProc->parentP->left){
	positionProc->parentP->left = save_right_Proc;
  } else {
	positionProc->parentP->right = save_right_Proc;
  }
  save_right_Proc->left = positionProc;
  positionProc->parentP = save_right_Proc;
}

/*
  rotateRight(struct RedBlackTree*, struct proc*)
  parameters:The red black tree pointer to access and modify the root, and the current process in the tree that will be rotated to the right
  returns:none
  This function will perform a rotation on the process structure in the tree that is passed into the function. 
  It will perform a right rotation, where it will move down rightward in the tree and its left process will be moved up to its place.
*/
void 
rotateRight(struct RedBlackTree* tree, struct proc* positionProc){
	
  struct proc* save_left_Proc = positionProc->left;
	
  positionProc->left = save_left_Proc->right;
	
  //Determine parents for the process being rotated
  if(save_left_Proc->right != 0)
	save_left_Proc->right->parentP = positionProc;
  save_left_Proc->parentP = positionProc->parentP;
  if(positionProc->parentP == 0){
	tree->root = save_left_Proc;
  } else if(positionProc == positionProc->parentP->right){
	positionProc->parentP->right = save_left_Proc;
  } else {
	positionProc->parentP->left = save_left_Proc;
  }
  save_left_Proc->right = positionProc;
  positionProc->parentP = save_left_Proc;
	
}

/*
  setMinimumVRuntimeproc(struct proc*)
  parameters: the address of a process in the tree to be utilized to traverse the tree
  returns:A pointer that contains the address to the process with the smallest Virtual Runtime
  This function will return a pointer to the address of the process with the smallest Virtual Runtime. 
  It will do this by traversing through the left branch of the tree until it reaches the process.
*/
struct proc*
setMinimumVRuntimeproc(struct proc* traversingProcess){
	
  if(traversingProcess != 0){
	if(traversingProcess->left != 0){
	    return setMinimumVRuntimeproc(traversingProcess->left);
	} else {
	    return traversingProcess;
	}
  }
	return 0;
}

struct proc*
insertproc(struct proc* traversingProcess, struct proc* insertingProcess){
	
  insertingProcess->color = RED;
	
  //i.e it is root or at leaf of tree
  if(traversingProcess == 0){
	return insertingProcess;
  }		
  //i.e everything after root
  //move process to the right of the current subtree
  if(traversingProcess->virtualRuntime <= insertingProcess->virtualRuntime){
	insertingProcess->parentP = traversingProcess;
	traversingProcess->right = insertproc(traversingProcess->right, insertingProcess);
  } else {
	insertingProcess->parentP = traversingProcess;		
	traversingProcess->left = insertproc(traversingProcess->left, insertingProcess);
  }
	
  return traversingProcess;
}

/*
  insertionCases(struct RedBlackTree*, struct proc*, int)
  parameters: the pointer of the tree, process in the red black tree and an integer value
  returns: none
  This function will contain different cases that will incorporate the properties for a red black tree. It will utilize the integer value to determine which case need to be handled.
  cases:
  -1: if the current inserted process is the root
  -2: if the current inserted process's parent is black
  -3: if both parent and uncle processes are red, then repaint them black
  -4: if parent is red and uncle is black, but current process is red and the current process is right child of parent that is left of grandparent or vice versa
  -5: same as case four but the current process is left child of parent that is left of grandparent or vice versa
*/
void
insertionCases(struct RedBlackTree* tree, struct proc* rbProcess, int cases){
	
  struct proc* uncle;
  struct proc* grandparent;
	
  switch(cases){
  case 1:
	if(rbProcess->parentP == 0)
		rbProcess->color = BLACK;
	else
		insertionCases(tree, rbProcess, 2);
	break;
	
  case 2:
	if(rbProcess->parentP->color == RED)
		insertionCases(tree, rbProcess, 3);
	break;
	
  case 3:
	uncle = retrieveUncleproc(rbProcess);
	
	if(uncle != 0 && uncle->color == RED){
		rbProcess->parentP->color = BLACK;
		uncle->color = BLACK;
		grandparent = retrieveGrandparentproc(rbProcess);
		grandparent->color = RED;
		insertionCases(tree, grandparent, 1);
		grandparent = 0;
	} else {
		insertionCases(tree, rbProcess,4);
	}
	
	uncle = 0;
	break;
  
  case 4:
	grandparent = retrieveGrandparentproc(rbProcess);
	
	if(rbProcess == rbProcess->parentP->right && rbProcess->parentP == grandparent->left){
		rotateLeft(tree, rbProcess->parentP);
		rbProcess = rbProcess->left;
	} else if(rbProcess == rbProcess->parentP->left && rbProcess->parentP == grandparent->right){
		rotateRight(tree, rbProcess->parentP);
		rbProcess = rbProcess->right;
	}
	insertionCases(tree, rbProcess, 5);
	grandparent = 0;
	break;
	
  case 5:
    grandparent = retrieveGrandparentproc(rbProcess);
	
	if(grandparent != 0){
		grandparent->color = RED;
		rbProcess->parentP->color = BLACK;
		if(rbProcess == rbProcess->parentP->left && rbProcess->parentP == grandparent->left){
			rotateRight(tree, grandparent);
		} else if(rbProcess == rbProcess->parentP->right && rbProcess->parentP == grandparent->right){
			rotateLeft(tree, grandparent);
		}
	}
	
	grandparent = 0;
	break;
	
  default:
	break;
  }
  return;
}

void
insertProcess(struct RedBlackTree* tree, struct proc* p){

  acquire(&tree->lock);
  if(!fullTree(tree)){	
	//actually insert process into tree
	tree->root = insertproc(tree->root, p);
	if(tree->count == 0)
		tree->root->parentP = 0;
    	tree->count += 1;
	
	//Calculate process weight
	p->weightValue = calculateWeight(p->niceValue);

	//perform total weight calculation 
	tree->rbTreeWeight += p->weightValue;
	
    	//Check for possible cases for Red Black tree property violations
	insertionCases(tree, p, 1);
		
	//This function call will find the process with the smallest vRuntime, unless 
	//there was no insertion of a process that has a smaller minimum virtual runtime then the process that is being pointed by min_vRuntime
	if(tree->min_vRuntime == 0 || tree->min_vRuntime->left != 0)
		tree->min_vRuntime = setMinimumVRuntimeproc(tree->root);
	 
  }	
  release(&tree->lock);
}

/*
  retrievingCases(struct RedBlackTree*, struct proc*, struct proc*, int)
  paramters: The red black tree pointer to access and modify the root, the parent of the process, the pointer to the process with the smallest virtual Runtime and the case number
  returns: none
  This function will check for violations of the red black tree to ensure the trees properties are not broken when we remove the process out of the tree. 
  cases:
  -1:We remove the process that needs to be retrieved and ensure that either the process or the process's child is red, but not both of them.
  -2:if both the process we want to remove is black and it has child that is black, then we would have to perform recolor and rotations to ensure red black tree property is met.
  
*/
void
retrievingCases(struct RedBlackTree* tree, struct proc* parentProc, struct proc* process, int cases){
  struct proc* parentProcess;
  struct proc* childProcess;
  struct proc* siblingProcess;
  
  switch(cases){
	case 1:
		//Replace smallest virtual Runtime process with its right child 
		parentProcess = parentProc;
		childProcess = process->right;
		
		//if the process being removed is on the root
		if(process == tree->root){
			
			tree->root = childProcess;
			if(childProcess != 0){
				childProcess->parentP = 0;
				childProcess->color = BLACK;
			}
			
		} else if(childProcess != 0 && !(process->color == childProcess->color)){
			//Replace current process by it's right child
			childProcess->parentP = parentProcess;
			parentProcess->left = childProcess;
			childProcess->color = BLACK;		
		} else if(process->color == RED){		
			parentProcess->left = childProcess;
		} else {	
			if(childProcess != 0)
				childProcess->parentP = parentProcess;
			
			
			parentProcess->left = childProcess;
			retrievingCases(tree, parentProcess, childProcess, 2);
		}
		
		process->parentP = 0;
		process->left = 0;
		process->right = 0;
		parentProcess = 0;
		childProcess = 0;
		break;
		
	case 2:
		
		//Check if process is not root,i.e parentProc != 0, and process is black
		while(process != tree->root && (process == 0 || process->color == BLACK)){
			
			////Obtain sibling process
			if(process == parentProc->left){
				siblingProcess = parentProc->right;
				
				if(siblingProcess != 0 && siblingProcess->color == RED){
					siblingProcess->color = BLACK;
					parentProc->color = RED;
					rotateLeft(tree, parentProc);
					siblingProcess = parentProc->right;
				}
				if((siblingProcess->left == 0 || siblingProcess->left->color == BLACK) && (siblingProcess->right == 0 || siblingProcess->right->color == BLACK)){
					siblingProcess->color = RED;
					//Change process pointer and parentProc pointer
					process = parentProc;
					parentProc = parentProc->parentP;
				} else {
					if(siblingProcess->right == 0 || siblingProcess->right->color == BLACK){
						//Color left child
						if(siblingProcess->left != 0){
							siblingProcess->left->color = BLACK;
						} 
						siblingProcess->color = RED;
						rotateRight(tree, siblingProcess);
						siblingProcess = parentProc->right;
					}
					
					siblingProcess->color = parentProc->color;
					parentProc->color = BLACK;
					siblingProcess->right->color = BLACK;
					rotateLeft(tree, parentProc);
					process = tree->root;
				}
			} 
		}
		if(process != 0)
			process->color = BLACK;
		
		break;
	
	default:
		break;
  }
  return;
	
}

struct proc*
retrieveProcess(struct RedBlackTree* tree, int latency, int min_granularity){
  struct proc* foundProcess;	//Process pointer utilized to hold the address of the process with smallest VRuntime 

  acquire(&tree->lock);
  if(!emptyTree(tree)){

	//If the number of processes are greater than the division between latency and minimum granularity
	//then recalculate the period for the processes
	//This condition is performed when the scheduler selects the next process to run
        //The formula can be found in CFS tuning article by Jacek Kobus and Refal Szklarski
	//In the CFS schduler tuning section:
	if(tree->count > (latency / min_granularity)){
		tree->period = tree->count * min_granularity;
	} 

	//retrive the process with the smallest virtual runtime by removing it from the red black tree and returning it
	foundProcess = tree->min_vRuntime;	

	//Determine if the process that is being chosen is runnable at the time of the selection, if it is not, then don't return it.
	if(foundProcess->state != RUNNABLE){
  		release(&tree->lock);
		return 0;
	}

	retrievingCases(tree, tree->min_vRuntime->parentP, tree->min_vRuntime, 1);
	tree->count -= 1;

	//Determine new process with the smallest virtual runtime
	tree->min_vRuntime = setMinimumVRuntimeproc(tree->root);

	//Calculate retrieved process's time slice based on formula: period*(process's weight/ red black tree weight)
	//Where period is the length of the epoch
	//The formula can be found in CFS tuning article by Jacek Kobus and Refal Szklarski
	//In the scheduling section:
	foundProcess->maximumExecutiontime = (tree->period * foundProcess->weightValue / tree->rbTreeWeight);
	
	//Recalculate total weight of red-black tree
	tree->rbTreeWeight -= foundProcess->weightValue;
  } else 
	foundProcess = 0;

  release(&tree->lock);
  return foundProcess;
}

////////