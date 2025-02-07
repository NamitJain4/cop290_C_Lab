#include <stdio.h>
#include <stdlib.h>

// not implementing min, max, sleep (yet)
// + -> plus, - -> minus, * -> mult, / -> div, + -> default value

struct Node {
    char *name;                 // name for eg. B1
    int value;                  // final calculated value
    struct Node **dependencies; // array of all node pointers dependent upon current node
    int depCount;               // length of the dependencies array
    char *opcode;               // opcode for ith dependency
    int constant;               // constant involved, for eg. 1 in the case of A1 = B1 + 1
    int old_val;                // old value, to update the dependencies based on that
};

/*
// intended to make a topsort (but too complicated and unnecessary)

void sort(struct Node *order[], int *length, struct Node *node) {
    node->visited = 1;
    for (int i = 0; i < node->depCount; i++) {
        if (!(node->dependencies[i]->visited)) {
            sort(order, length, node->dependencies[i]);
        }
    }
    order[(*length)++] = node;
}
*/

struct Node* updateNode(struct Node *node) {
    for (int i = 0; i < node->depCount; i++) {
        changeValOfDep(node, node->dependencies[i], node->opcode[i]);
    }
    
    return node;
}

void changeValOfDep(struct Node *node, struct Node *dep, char opcode) {
    if (opcode == '+') {
        dep->value += node->value - node->old_val;
    }
    else if (opcode == '-') {
        dep->value += node->old_val - node->value; 
    }
}