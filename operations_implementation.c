#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include <unistd.h> // this library is only available on unix based systems and we are allowed to 
                    // use it as mentioned on piazza --for sleep function
// to run sleep on unix comment out above line
// to run sleep on windows comment out below 2 lines                    
#include <windows.h>
#define sleep(x) Sleep(1000*(x))



// even though the logic is complete and bug free (hopefully)
// ill making some edits in the file here and there, to make it faster and more memory efficient
// ~ namit


/*
ALL OPERATIONS IMPLEMENTED
TOPOLOGICAL SORT IMPLEMENTED TO OPTIMIZE THE TIME COMPLEXITY
MADE 2 TESTCASES (DIDN'T TEST ALL OPERATIONS)


A1 = 2 - B1
A1 = B1 - 2

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
    int visited;                  // for topsort
};

void updateNode(struct Node *node, struct Node **dep_upon, int dCount, char opcode, int new_constant);
int calcValue(struct Node *node);
void printNodeDetails(struct Node *node);
void sort(struct Node **order[], int *length, struct Node *node);
void set_visited_to_zero(struct Node *order[], int length);



// taking a pointer to a pointer to a node array
// (because realloc may change the address of the array of pointers entirely, and that wouldn't be reflected outside the function)
void sort(struct Node **order[], int *length, struct Node *node) {
    node->visited = 1;
    for (int i = 0; i < node->depCount; i++) {
        if (!(node->dependencies[i]->visited)) {
            sort(order, length, node->dependencies[i]);
        }
    }
    // append the node to the order array, by increasing its size too
    // printf("%s %d\n", node->name, *length);
    *order = realloc(*order, (*length + 1) * sizeof(struct Node*));
    (*order)[(*length)++] = node;
}

void set_visited_to_zero(struct Node *order[], int length) {
    for (int i = 0; i < length; i++) {
        order[i]->visited = 0;
    }
}

void updateNode(struct Node *node, struct Node **dep_upon, int dCount, char opcode, int new_constant) {
    // A1 = 2 - B1, expecting dep_upon = [NULL, B1], dCount = 2
    // A1 = 2, dCount = 0 and dep_upon = []
    // A1 = B1, new_constant should be 0, opcode should be +

    // traverse through the dep_upon array
    for (int i = 0; i < node->dCount; i++) {
        // for each element in dep_upon, remove node from its dependencies array
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
    node->value = calcValue(node);

    // traverse through the nodes in the dep_upon array
    for (int i = 0; i < node->dCount; i++) {
        // update the dependencies of the nodes in the dep_upon array (add the node)
        struct Node *dep = node->dependent_upon[i];
        dep->dependencies = realloc(dep->dependencies, (dep->depCount + 1) * sizeof(struct Node*));
        dep->dependencies[dep->depCount++] = node;
    }

    // debugging
    //printNodeDetails(node);

    struct Node **order = malloc(sizeof(struct Node *));
    int length = 0;
    sort(&order, &length, node);
    set_visited_to_zero(order, length);
    /*for (int i = 0; i < length; i++) {
        printf("%s ", order[i]->name);
    }
    printf("\n \t%d \n", length);*/
    
    for (int i = length - 1; i >= 0; i--) {
        order[i]->value = calcValue(order[i]);
    }
    free(order);
}

int calcValue(struct Node *node) {
    if (node->dCount == 0) {
        if (node->opcode == 'l'){
            sleep(node->constant);
        }
        return node->constant; // No dependencies, value equals constant
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
                printf("Error: Division by zero\n");
                return 0;
            }
        }
        if (node->constant != 0) {
            return node->dependent_upon[0]->value / node->constant;
        } else {
            printf("Error: Division by zero\n");
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
        double mean = 0;
        for (int i = 0; i < node->dCount; i++) {
            mean += node->dependent_upon[i]->value;
        }
        mean /= node->dCount;
        double variance = 0;
        for (int i = 0; i < node->dCount; i++) {
            variance += pow(node->dependent_upon[i]->value - mean, 2);
        }
        variance /= node->dCount;
        return (int)sqrt(variance);
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
        sleep(node->dependent_upon[0]->value);
        return node->dependent_upon[0]->value;
    } else {
        printf("Error: Unknown opcode\n");
        return 0;
    }
}

void printNodeDetails(struct Node *node) {
    printf("Node %s:\n", node->name);
    printf("  Value: %d\n", node->value);
    printf("  Dependencies: ");
    for (int i = 0; i < node->depCount; i++) {
        printf("%s ", node->dependencies[i]->name);
    }
    printf("\n  Dependent upon: ");
    for (int i = 0; i < node->dCount; i++) {
        printf("%s ", node->dependent_upon[i]->name);
    }
    printf("\n");
}

int main() {
    printf("-------------------- TC - 1 --------------------\n");
    /*
    B1 = 3
    B2 = 4
    A1 = B1 + B2
    C1 = A1 * B2
    D1 = A1 * C1
    C1 = B2 + 8
    B1 = 12
    16 12 4 12 16
    */

    // Create nodes on the heap
    struct Node *A1 = malloc(sizeof(struct Node));
    struct Node *B1 = malloc(sizeof(struct Node));
    struct Node *B2 = malloc(sizeof(struct Node));
    struct Node *C1 = malloc(sizeof(struct Node));
    struct Node *D1 = malloc(sizeof(struct Node));

    // Initialize nodes
    *A1 = (struct Node){"A1", 0, NULL, 0, NULL, 0, '+', 0, 0, 0};
    *B1 = (struct Node){"B1", 0, NULL, 0, NULL, 0, '+', 0, 0, 0};
    *B2 = (struct Node){"B2", 0, NULL, 0, NULL, 0, '+', 0, 0, 0};
    *C1 = (struct Node){"C1", 0, NULL, 0, NULL, 0, '+', 0, 0, 0};
    *D1 = (struct Node){"D1", 0, NULL, 0, NULL, 0, '+', 0, 0, 0};

    // Initialize values using updateNode
    updateNode(B1, NULL, 0, '+', 3);
    updateNode(B2, NULL, 0, '+', 4);

    // Set dependencies
    struct Node* A1_deps[] = {B1, B2};
    struct Node* C1_deps[] = {A1, B2};
    struct Node* D1_deps[] = {A1, C1};
    struct Node* C1_deps_2[] = {B2};

    // Update nodes
    updateNode(A1, A1_deps, 2, '+', 0);
    updateNode(C1, C1_deps, 2, '*', 0);
    updateNode(D1, D1_deps, 2, 'M', 0);
    updateNode(C1, C1_deps_2, 1, '+', 8);
    updateNode(B1, NULL, 0, '+', 12);
    // 16 12 4 12 16

    // Print results
    printNodeDetails(A1);
    printNodeDetails(B1);
    printNodeDetails(B2);
    printNodeDetails(C1);
    printNodeDetails(D1);

    // Free allocated memory
    free(A1->dependencies);
    free(A1->dependent_upon);
    free(A1);
    free(B1->dependencies);
    free(B1->dependent_upon);
    free(B1);
    free(B2->dependencies);
    free(B2->dependent_upon);
    free(B2);
    free(C1->dependencies);
    free(C1->dependent_upon);
    free(C1);
    free(D1->dependencies);
    free(D1->dependent_upon);
    free(D1);

    printf("\n\n-------------------- TC - 2 --------------------\n");

    struct Node *A1_ = malloc(sizeof(struct Node));
    struct Node *B1_ = malloc(sizeof(struct Node));
    struct Node *B2_ = malloc(sizeof(struct Node));
    struct Node *B3_ = malloc(sizeof(struct Node));
    struct Node *B4_ = malloc(sizeof(struct Node));

    // Initialize nodes
    *A1_ = (struct Node){"A1", 0, NULL, 0, NULL, 0, '+', 0, 0, 0};
    *B1_ = (struct Node){"B1", 0, NULL, 0, NULL, 0, '+', 0, 0, 0};
    *B2_ = (struct Node){"B2", 0, NULL, 0, NULL, 0, '+', 0, 0, 0};
    *B3_ = (struct Node){"B3", 0, NULL, 0, NULL, 0, '+', 0, 0, 0};
    *B4_ = (struct Node){"B4", 0, NULL, 0, NULL, 0, '+', 0, 0, 0};

    /*
    A1 = 3
    B1 = A1 + 1
    B3 = B1 * 2
    B2 = A1 - B3
    B4 = A1 + B2
    A1 = 4
    4 5 -6 10 -2
    */ 
    updateNode(A1_, NULL, 0, '+', 3);
    struct Node* B1_deps_[] = {A1_};
    updateNode(B1_, B1_deps_, 1, '+', 1);
    struct Node* B3_deps_[] = {B1_};
    updateNode(B3_, B3_deps_, 1, '*', 2);
    struct Node* B2_deps_[] = {A1_, B3_};
    updateNode(B2_, B2_deps_, 2, '-', 0);
    struct Node* B4_deps_[] = {A1_, B2_};
    updateNode(B4_, B4_deps_, 2, '+', 0);
    updateNode(A1_, NULL, 0, '+', 4);

    printNodeDetails(A1_);
    printNodeDetails(B1_);
    printNodeDetails(B2_);
    printNodeDetails(B3_);
    printNodeDetails(B4_);

    // Free allocated memory
    free(A1_->dependencies);
    free(A1_->dependent_upon);
    free(A1_);
    free(B1_->dependencies);
    free(B1_->dependent_upon);
    free(B1_);
    free(B2_->dependencies);
    free(B2_->dependent_upon);
    free(B2_);
    free(B3_->dependencies);
    free(B3_->dependent_upon);
    free(B3_);
    free(B4_->dependencies);
    free(B4_->dependent_upon);
    free(B4_);

    return 0;
}
