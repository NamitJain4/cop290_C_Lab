#include <stdio.h>
#include <stdlib.h>

// not implementing min, max, sleep (yet)
// + -> plus, - -> minus, * -> mult, / -> div, + -> default value



/*
A1 = 2 - B1
A1 = B1 - 2

                      opcode
A1 = B1 - B2        -
A1 = B1 + B2        +
A1 = B1 * B2        *
A1 = B1 / B2        /
A1 = max(B1, B2)    M
A1 = min(B1, B2)    m
A1 = sleep(B1)      l    => please handle sleep
A1 = stdev(B1, B2)  S
A1 = avg(B1, B2)    a
A1 = sum(B1, B2)    s
*/

struct Node {
    char *name;                   // name for eg. B1
    int value;                    // final calculated value
    struct Node **dependencies;   // array of all node pointers dependent upon current node
    int depCount;                 // length of the dependencies array
    struct Node **dependent_upon; // all the nodes in the rhs such that current node was in the lhs
    int dCount;                   // number of nodes the current node is dependent upon
    char opcode;                  // opcode for dep_upon
    int constant;                 // constant involved, for eg. 1 in the case of A1 = B1 + 1 (toh A1 ka constant is 1)
    int old_val;                  // old value, to update the dependencies based on that
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

void updateNode(struct Node *node, struct Node **dep_upon, int dCount, char opcode, int new_constant) {
    // A1 = 2 - B1, expecting dep_upon = [NULL, B1], dCount = 2
    // A1 = 2, dCount = 0 and dep_upon = []
    // A1 = B1, new_constant should be 0, opcode should be +

    // traverse through the dep_upon array
    for (int i = 0; i < node->dCount; i++) {
        // for each element in dep_upon, remove node from its dependencies array
        struct Node *par = node->dependent_upon[i];
        struct Node **new_dependencies = malloc((par->depCount - 1) * sizeof(struct Node*));
        for (int j = 0, k = 0; j < par->depCount; j++) {
            if (par->dependencies[j] != node) {
                new_dependencies[k++] = par->dependencies[j];
            }
        }
        free(par->dependencies);
        par->dependencies = new_dependencies;
        par->depCount--;
    }
    // empty the dep_upon array
    free(node->dependent_upon);
    node->dependent_upon = NULL;

    // update the opcode of node
    node->opcode = opcode;

    // equate dep_upon to the dep_upon of the node
    node->dependent_upon = dep_upon;
    node->dCount = dCount;

    // update constant
    node->constant = new_constant;

    // change value of the current node
    node->value = calcValue(node);

    // traverse through the nodes in the dep_upon array
    for (int i = 0; i < node->dCount; i++) {
        // update the dependencies of the nodes in the dep_upon array (add the node)
        struct Node *dep = node->dependent_upon[i];
        dep->dependencies = realloc(dep->dependencies, (dep->depCount + 1) * sizeof(struct Node*));
        dep->dependencies[dep->depCount++] = node;
    }

}

int calcValue(struct Node *node) {
    if (node->dCount == 0) {
        return node->constant; // No dependencies, value is the constant
    }

    switch (node->opcode) {
        case '+':
            if (node->dCount == 2) {
                return node->dependent_upon[0]->value + node->dependent_upon[1]->value;
            } else {
                return node->dependent_upon[0]->value + node->constant;
            }
        case '-':
            return node->dependent_upon[0]->value - node->dependent_upon[1]->value;
        case '*':
            return node->dependent_upon[0]->value * node->dependent_upon[1]->value;
        case '/':
            if (node->dependent_upon[1]->value != 0) {
                return node->dependent_upon[0]->value / node->dependent_upon[1]->value;
            } else {
                printf("Error: Division by zero\n");
                return 0;
            }
        case 'M':
            return (node->dependent_upon[0]->value > node->dependent_upon[1]->value) ? node->dependent_upon[0]->value : node->dependent_upon[1]->value;
        case 'm':
            return (node->dependent_upon[0]->value < node->dependent_upon[1]->value) ? node->dependent_upon[0]->value : node->dependent_upon[1]->value;
        case 'l':
            // Handle sleep operation if needed
            break;
        case 'S':
            // Handle standard deviation calculation if needed
            break;
        case 'a':
            return (node->dependent_upon[0]->value + node->dependent_upon[1]->value) / 2;
        case 's':
            return node->dependent_upon[0]->value + node->dependent_upon[1]->value;
        default:
            printf("Error: Unknown opcode\n");
            return 0;
    }
    return 0; // Default return value if no case matches
}

struct Node* changeDeps(struct Node *node) {
    for (int i = 0; i < node->depCount; i++) {
        changeValOfDep(node, node->dependencies[i]);
    }
    
    return node;
}

void changeValOfDep(struct Node *node, struct Node *dep) {
    char opcode = dep->opcode;
    if (opcode == '+' || opcode == 's' || (opcode == '-' && dep->dependent_upon[0] == node)) {
        dep->value += node->value - node->old_val;
    }
    else if (opcode == '-') {
        dep->value += node->old_val - node->value; 
    }
    else if (opcode == '*') {
        if (dep->dCount == 1) {
            dep->value += (node->value - node->old_val) * dep->constant;
        }
        else {
            if (node == dep->dependent_upon[0]) {
                dep->value += (node->value - node->old_val) * dep->dependent_upon[1]->value;
            }
            else {
                dep->value += (node->value - node->old_val) * dep->dependent_upon[0]->value;
            }
        }
    }
}

/*
A1 = 3
A2 = 4
B1 = A1*A2 = 12
A1 = 5
B1_old + (A1_new - A1_old) * A2 = 12 + 2 * 4 = 20
*/
