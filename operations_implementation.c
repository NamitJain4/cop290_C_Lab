#include "common.h"

extern char *exec_status;
extern char *ERR2;

/*
                    opcode
A1 = B1 - B2        -
A1 = B1 + B2        +
A1 = B1 * B2        *
A1 = B1 / B2        /
A1 = max(B1, B2)    M
A1 = min(B1, B2)    m
A1 = sleep(B1)      l
A1 = stdev(B1, B2)  S
A1 = avg(B1, B2)    a
A1 = sum(B1, B2)    s
*/

int calcValue(struct Node *node);
void printNodeDetails(struct Node *node);
void sort(struct Node **order[], int *length, struct Node *node, int *cycle);
void set_visited_to_zero(struct Node *order[], int length);
void set_node_to_error(struct Node *node);

// taking a pointer to a (pointer to a node) array
// (because realloc may change the address of the array of pointers entirely, and that wouldn't be reflected outside the function)
void sort(struct Node **order[], int *length, struct Node *node, int *cycle) {
    if (node->visited == 1) {
        *cycle = 1;
        return;
    }
    if (node->visited == 2) {
        return;
    }
    node->visited = 1;
    for (int i = 0; i < node->depCount; i++) {
        if (*cycle) {
            return;
        }
        sort(order, length, node->dependencies[i], cycle);
    }
    node->visited = 2;
    *order = realloc(*order, (*length + 1) * sizeof(struct Node*));
    (*order)[(*length)++] = node;
}

void set_visited_to_zero(struct Node *order[], int length) {
    for (int i = 0; i < length; i++) {
        order[i]->visited = 0;
    }
}

// node => pointer to the node that you want to change,
// dep_upon => array of pointers to the nodes that the current node is going to be dependent upon,
// dCount => number of nodes in the dep_upon array,
// opcode => opcode,
// new_constant => new constant
void updateNode(struct Node *node, struct Node **dep_upon, int dCount, char opcode, int new_constant) {
    // A1 = 2 - B1, expecting dep_upon = [NULL, B1], dCount = 2
    // A1 = 2, dCount = 0 and dep_upon = [], new_constant = 2, opcode = +
    // A1 = B1, new_constant should be 0, opcode should be +
    // A1 = 2 / B1, new_constant = 2, opcode = /, dep_upon = [NULL, B1], dCount = 2
    // A1 = B1 / 2, new_constant = 2, opcode = /, dep_upon = [B1], dCount = 1
    // A1 = B1 / B2, new_constant = 0, opcode = /, dep_upon = [B1, B2], dCount = 2
    // A1 = max(B1:C2), dep_upon = [C2, B1, B2, C1] (any order), dCount = 4
    // A1 = 2 * A1, dep_upon = [A1], dCount = 1
    // A1 = 2 * 3, dep_upon = [], dCount = 0, new_constant = 6
    // note: in the above example, you must calculate 2*3 and store it in new_constant
    // for sleep ask ayush
    // max, min, stdev, avg, sum => dCount >= 2, dep_upon = [start node, end node]


    struct Node **order = malloc(sizeof(struct Node *));
    int length = 0;
    int cycle = 0;
    for (int i = 0; i < dCount; i++) {
        if (dep_upon[i] != NULL)
            dep_upon[i]->visited = 1;   // (fix) dep_upon[i] can be NULL also
    }
    sort(&order, &length, node, &cycle);
    set_visited_to_zero(order, length);
    for (int i = 0; i < dCount; i++) {
        if (dep_upon[i] != NULL)
            dep_upon[i]->visited = 0;
    }
    if (cycle) {
        exec_status = ERR2;
        free(order);
        return;
    }

    // traverse through the dep_upon array
    for (int i = 0; i < node->dCount; i++) {
        // for each element in dep_upon, remove node from its dependencies array
        if (node->dependent_upon[i] == NULL) {
            continue;
        }
        struct Node *par = node->dependent_upon[i];
        int j;
        for (j = 0; j < par->depCount; j++) {
            if (par->dependencies[j] == node) {
                break;
            }
        }
        for (int k = j; k < par->depCount - 1; k++) {
            par->dependencies[k] = par->dependencies[k + 1];
        }
        par->dependencies = realloc(par->dependencies, (--(par->depCount)) * sizeof(struct Node *));
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
    // node->value = calcValue(node);

    // traverse through the nodes in the dep_upon array
    for (int i = 0; i < node->dCount; i++) {
        // update the dependencies of the nodes in the dep_upon array (add the node)
        struct Node *dep = node->dependent_upon[i];
        if (dep == NULL) {
            continue;
        }
        dep->dependencies = realloc(dep->dependencies, (dep->depCount + 1) * sizeof(struct Node*));
        dep->dependencies[dep->depCount++] = node;
    }

    // debugging
    //printNodeDetails(node);
    /*for (int i = 0; i < length; i++) {
        printf("%s %d  ", order[i]->name, order[i]->value);
    }
    printf("\n \t%d \n", length);*/
    
    for (int i = length - 1; i >= 0; i--) {
        order[i]->value = calcValue(order[i]);
    }
}

void set_node_to_error(struct Node *node) {
    node->is_err = 1;
    // free(node->dependencies);                           // (fix) dependencies should not be free they also should be
    // node->dependencies = NULL;                          // set to err and dont remove dependencies
    // node->depCount = 0;
}

int calcValue(struct Node *node) {
    if (node->is_err) {
        // free(node->dependencies);                       // (fix) dependencies should not be free they also should be
        // node->dependencies = NULL;                      // set to err and dont remove dependencies
        // node->depCount = 0;        
    }
    node->is_err = 0;

    if (node->dCount == 0) {
        if (node->opcode == 'l'){
            if (node->constant > 0){
                sleep(node->constant);
            }            
        }
        return node->constant; // No dependencies, value equals constant
    }

    for (int i = 0; i < node->dCount; i++) {
        if (node->dependent_upon[i] != NULL && node->dependent_upon[i]->is_err) {
            set_node_to_error(node);
            return 0;
        }
    }

    if (node->opcode == '+') {
        if (node->dCount == 2) {
            return node->dependent_upon[0]->value + node->dependent_upon[1]->value;
        }
        return node->dependent_upon[0]->value + node->constant;
    } else if (node->opcode == '-') {
        if (node->dCount == 2) {
            if (node->dependent_upon[0]) {
                return node->dependent_upon[0]->value - node->dependent_upon[1]->value;
            }
            return node->constant - node->dependent_upon[1]->value;
        }
        return node->dependent_upon[0]->value - node->constant;
    } else if (node->opcode == '*') {
        if (node->dCount == 2) {
            return node->dependent_upon[0]->value * node->dependent_upon[1]->value;
        }
        return node->dependent_upon[0]->value * node->constant;
    } else if (node->opcode == '/') {
        if (node->dCount == 2) {
            if (node->dependent_upon[1]->value != 0) {
                if (node->dependent_upon[0]) {
                    return node->dependent_upon[0]->value / node->dependent_upon[1]->value;
                }
                return node->constant / node->dependent_upon[1]->value;
            } else {
                // printf("Error: Division by zero occured while calculating node: %s\n", node->name);
                set_node_to_error(node);
                return 0;
            }
        }
        if (node->constant != 0) {
            return node->dependent_upon[0]->value / node->constant;
        } else {
            // printf("Error: Division by zero occured while calculating node: %s\n", node->name);
            set_node_to_error(node);
            return 0;
        }
    } else if (node->opcode == 'M') {
        int M = node->dependent_upon[0]->value;
        for (int i = 1; i < node->dCount; i++) {
            if (M < node->dependent_upon[i]->value) {
                M = node->dependent_upon[i]->value;
            }
        }
        return M;
    } else if (node->opcode == 'm') {
        int m = node->dependent_upon[0]->value;
        for (int i = 1; i < node->dCount; i++) {
            if (m > node->dependent_upon[i]->value) {
                m = node->dependent_upon[i]->value;
            }
        }
        return m;
    } else if (node->opcode == 'S') {
        int mean = 0;
        for (int i = 0; i < node->dCount; i++) {
            mean += node->dependent_upon[i]->value;
        }
        mean /= node->dCount;
        double variance = 0;
        for (int i = 0; i < node->dCount; i++) {
            variance += (node->dependent_upon[i]->value - mean) * (node->dependent_upon[i]->value - mean);
        }
        variance /= node->dCount;
        return (int) round(sqrt(variance));
    } else if (node->opcode == 'a') {
        int sum = 0;
        for (int i = 0; i < node->dCount; i++) {
            sum += node->dependent_upon[i]->value;
        }
        return sum / node->dCount;
    } else if (node->opcode == 's') {
        int total = 0;
        for (int i = 0; i < node->dCount; i++) {
            total += node->dependent_upon[i]->value;
        }
        return total;
    } else if (node->opcode == 'l'){
        if (node->dependent_upon[0]->value > 0){
            sleep(node->dependent_upon[0]->value);
        }        
        return node->dependent_upon[0]->value;
    } else {
        // printf("Error: Unknown opcode\n");
        return 0;
    }
}
