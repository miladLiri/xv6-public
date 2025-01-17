<p align="center">  
  <img src="badges/Screenshot%20from%202023-12-04%2019-22-59.png" alt="image" width="700" height="300">
</p>

<p align="center">  
  <a href="#getting-started">Getting Started</a> • 
  <a href="#code-explanation">Code Explanation</a> •
  <a href="#contact">Contact</a> •
  <a href="#contribution"> Contribution</a>
</p>


## Introduction
Effective process scheduling and optimized data structures are fundamental components of any operating system kernel. This project aims to enhance the process management capabilities of the xv6 kernel forked from a [reputable repository](https://github.com/mit-pdos/xv6-public) by implementing the Completely Fair Scheduler (CFS) algorithm and integrating it with the Red-Black Tree data structure

### Goals

+ Implementing Completely Fair Scheduler (CFS) on xv6: The project aims to integrate the Completely Fair Scheduler algorithm into the xv6 operating system. This involves designing and implementing the necessary data structures and scheduling policies to ensure fair resource allocation among processes.
+ Enhancing scheduler functionality with Red-Black Tree data structure: The project aims to utilize the Red-Black Tree data structure to enhance the functionality of the scheduler. By employing this balanced binary search tree, the scheduler can efficiently manage and prioritize processes based on specified criteria such as priority levels or other relevant metrics.

### Methodology

1. Understanding the CFS Algorithm: In this phase, we will thoroughly study the concepts, principles, and implementation details of the CFS algorithm.
2. Adapting the CFS Algorithm to xv6: This phase involves modifying the existing xv6 kernel to replace its default scheduler with the CFS algorithm. We will analyze the existing scheduler implementation and integrate the CFS algorithm while ensuring compatibility with the xv6 architecture.
3. Implementing the Red-Black Tree Data Structure: To efficiently manage processes and facilitate fast process lookup, insertion, and removal, we will integrate the Red-Black Tree data structure into the xv6 kernel. This self-balancing binary search tree will provide efficient access and management of process control blocks.
4. Testing and Evaluation: We will thoroughly test the modified xv6 kernel with the integrated CFS scheduler and Red-Black Tree data structure.

<p align="right">
    <a href="#introduction">(back up)</a>
</p>

## Getting Started

The process of getting started with this project is just like the steps involved in running the xv6 kernel.

1. First, make sure to have all the dependencies :
   ```
   user@user: ~$ sudo apt-get update && sudo apt-get install
    --yes build-essential git qemu-system-x86
   ```
2. After successfully installing the required programs, clone the Github repository of xv6 source code:
    ```
    user@user: ~$ git clone https: // github . com / mit-pdos /
    xv6-public
    ```
3. Now just compile the kernel:
    ```
    user@user: ~$ make qemu
    ```
<p align="right">
    <a href="#introduction">(back up)</a>
</p>

## Code Explanation
This section aims to outline the important modifications made to the codebase.
### Red Black Tree
We have made some changes to the Red-Black Tree implementation.
We have add insertion and deletion methods. Actually the deletion method was not useful.

We have also added some fields on proc struct to enhance the functionality:

        //CFS fields
        int virtualRuntime;    	
        int currentRuntime;				
        int maximumExecutiontime;	
        int niceValue;		
        int weightValue;

        //rbt fields
        enum Color color;
        struct proc *left;
        struct proc *right;
        struct proc *parentP;

### proc.c

in this section the changes made on **proc.c** are listed:

+ We have incorporated the Red-Black Tree initialization into the "pinit" function. This function takes on the responsibility of initializing processes. 
+ We have modified the "allocproc" function to incorporate the addition of new processes. Furthermore, we have implemented process initialization for the newly added processes, ensuring they start with default values.
    
        //cfs
        p->virtualRuntime = 0;
        p->currentRuntime = 0;
        p->maximumExecutiontime = 0;
        p->niceValue = 0;

        p->left = 0;
        p->right = 0;
        p->parentP = 0;

#### userinit
we have made some modifications on this block of code to gain some functionalities:

  1. The generation of the first process is the responsibility of this block
  2. The other processes will be forked from this process 
  3. After process generation, runnable processes will be inserted into the tree in this block of code

#### scheduler

Previously, when an interrupt occurred, the scheduler would simply preempt the current process. However, in the recent modification, the scheduler now employs a smarter strategy. It checks if there is a process with a shorter virtual time available. If a process with a shorter virtual time is found, it preempts the current process and replaces it. On the other hand, if no such process is found, it seamlessly continues the execution of the current process.

### trap.c
 After modification of this block of code, the runtime will be extended by one unit before calling the yield function



<p align="right">
    <a href="#introduction">(back up)</a>
</p>

## Contribution

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement". Don't forget to give the project a star! Thanks again!

1. Fork it (https://github.com/miladLiri/xv6-public/fork)
2. Create your feature branc (git checkout -b feature/fooBar)
3. Commit your changes (git commit -am 'Add some fooBar')
4. Push to the branch (git push origin feature/fooBar)
5. Create a new Pull Request

<p align="right">
    <a href="#introduction">(back up)</a>
</p>

## Contact

**Milad liri**

[![LinkedIn](https://img.shields.io/badge/linkedin-%230077B5.svg?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/milad-liri/)

**Aidin Asl Zaeim**

[![LinkedIn](https://img.shields.io/badge/linkedin-%230077B5.svg?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/aidinzaeim/)

<p align="right">
    <a href="#introduction">(back up)</a>
</p>
