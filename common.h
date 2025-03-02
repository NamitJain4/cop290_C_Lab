#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h> // this library is only available on unix based systems and we are allowed to 
                    // use it as mentioned on piazza --for sleep function
// to run sleep on unix comment out above line
// to run sleep on windows comment out below 2 lines                    
// #include <windows.h>
// #define sleep(x) Sleep(1000*(x))

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

// Node structure
struct Node {
    char *name;                   // name for eg. B1
    int value;                    // final calculated value
    struct Node **dependencies;   // array of all node pointers dependent upon current node
    int depCount;                 // length of the dependencies array
    struct Node **dependent_upon; // all the nodes in the rhs such that current node was in the lhs
    int dCount;                   // number of nodes the current node is dependent upon
    char opcode;                  // opcode for dep_upon
    int constant;                 // constant involved, for eg. 1 in the case of A1 = B1 + 1 (toh A1 ka constant is 1)
    int visited;                  // for topsort
    int is_err;                   // to check if the node is an error node
};

// External variables
extern int nrows; // (1..)
extern int ncols; // (1..)
extern int print_arr[9];
extern char *exec_status;
extern char *ERR2;
extern struct Node **lookup;

// Function declarations
void printSheet(void);
void updateNode(struct Node *node, struct Node **dep_upon, int dCount, char opcode, int new_constant);

#endif // COMMON_H
