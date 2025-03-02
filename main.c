/* Scanner + Parser */
#include "common.h"

/* Variables */
float exec_time = 0.0;
char *exec_status = "ok";
char *cmd;
char *gp;
int cmdarr[7] = {0, 0, 0, 0, 0, 0, 0};
int col = 0;
int row = 0;

int print_arr[9] = {0, 0, 0, 0, 0, 0, 0, 1, 1};

struct Node **lookup;

/* Constants */
char *OK = "ok";
char *ERR = "ParserEngineFailure";

/* Spreadsheet params */
int nrows;
int ncols;

/* Prototypes */
int parser(void);
int parse_cell(void);
int parse_int(void);
int parse_uint(void);
int process_cmdarr(void);
void nalloc(int col, int row);

int main(int argc, char *argv[])
{
    if (argc != 3) return -1;
    gp = argv[1];
    if (parse_uint() || *gp || (nrows = row) > 999) return -1;
    gp = argv[2];
    if (parse_uint() || *gp || (ncols = row) > 18278) return -1;

    char c; int i; int len; int cap;

    lookup = (struct Node **)malloc(nrows * ncols * sizeof(struct Node *));
    if (lookup == NULL) return -1;
    for (i = 0; i < nrows * ncols; i++)
        lookup[i] = NULL;

    while (1) {
        printSheet();
        /* Display prompt */
        printf("[%.1f] (%s) > ", exec_time, exec_status);

        /* Read input */
        len = 0; cap = 1;
        cmd = (char *)malloc(cap * sizeof(char));
        if (cmd == NULL) return -1;
        while ((c = getchar()) != '\n') {
            if (len == (cap - 1)) {
                cap *= 2;
                cmd = (char *)realloc(cmd, cap * sizeof(char));
                if (cmd == NULL) return -1;
            }
            cmd[len++] = c;
        }
        cmd[len] = '\0';

        /* Initialize */
        for (i = 0; i < 7; i++)
            cmdarr[i] = 0;
        for (i = 1; i < 7; i++)
            print_arr[i] = 0;

        /* Parse input */
        // printf("%s\n", cmd);
        c = parser();
        /*
        printf("cmdarr[]:");
        for (int i = 0; i < 7; i++)
            printf(" %d", cmdarr[i]);
        printf("\n%d\n", c);
        */
        free(cmd);

        /* Process return status of parser */
        switch(c) {
            case 0:
                exec_status = OK;
                process_cmdarr();
                break;
            case 1:
                exec_status = ERR;
                break;
        }

        /* Quit */
        if (cmdarr[0] == -1)
            break;
    }

    return 0;
}

int parser()
{
    gp = cmd;
    int status = 1; char *temp;

    /* Cmd test */
    if ((status = strcmp(gp, "q")) == 0)
        cmdarr[0] = -1;
    else if ((status = strcmp(gp, "w")) == 0)
        print_arr[6] = 'w';
    else if ((status = strcmp(gp, "d")) == 0)
        print_arr[6] = 'd';
    else if ((status = strcmp(gp, "a")) == 0)
        print_arr[6] = 'a';
    else if ((status = strcmp(gp, "s")) == 0)
        print_arr[6] = 's';
    else if ((status = strcmp(gp, "disable_output")) == 0)
        print_arr[0] = 1;
    else if ((status = strcmp(gp, "enable_output")) == 0)
        print_arr[0] = 0;
    print_arr[5] = (print_arr[6] != 0);
    if (status == 0)
        return 0;

    if (strncmp(gp, "scroll_to ", 10) == 0) {
        gp += 10;
        if (parse_cell() || *gp++ != '\0')
            return 1;
        print_arr[1] = 1; print_arr[3] = row; print_arr[4] = col;
        return 0;
    }
    
    /* Formula parsing */
    if (parse_cell() || *gp++ != '=')
        return 1;
    cmdarr[0] = col; cmdarr[1] = row;

    /* Expression parsing */
    status = 1; temp = gp;
    if (status && (status = parse_cell()))
        gp = temp;
    if (status && (status = parse_int()))
        gp = temp;
    if (status == 0) {
        cmdarr[3] = col; cmdarr[4] = row;
        /* Simple assignment expression */
        if (*gp == '\0') {
            cmdarr[2] = 0;
            return 0;
        }
        /* Arithmetic expression */
        else if (*gp == '+')
            cmdarr[2] = 1;
        else if (*gp == '-')
            cmdarr[2] = 2;
        else if (*gp == '*')
            cmdarr[2] = 3;
        else if (*gp == '/')
            cmdarr[2] = 4;
        else
            return 1;
        status = 1; temp = ++gp;
        if (status && (status = parse_cell()))
            gp = temp;
        if (status && (status = parse_int()))
            gp = temp;
        if (status || *gp++ != '\0')
            return 1;
        cmdarr[5] = col; cmdarr[6] = row;
        return 0;
    }
    /* Function */
    else {
        if (strncmp(gp, "MIN(", 4) == 0)
            cmdarr[2] = 5;
        else if (strncmp(gp, "MAX(", 4) == 0)
            cmdarr[2] = 6;
        else if (strncmp(gp, "AVG(", 4) == 0)
            cmdarr[2] = 7;
        else if (strncmp(gp, "SUM(", 4) == 0)
            cmdarr[2] = 8;
        else if (strncmp(gp, "STDEV(", 6) == 0)
            cmdarr[2] = 9;
        else if (strncmp(gp, "SLEEP(", 6) == 0)
            cmdarr[2] = 10;
        else
            return 1;

        if (cmdarr[2] < 9)
            gp += 4;
        else
            gp += 6;
        
        if (cmdarr[2] == 10) {
            status = 1; temp = gp;
            if (status && (status = parse_cell()))
                gp = temp;
            if (status && (status = parse_int()))
                gp = temp;
            if (status || *gp++ != ')')
                return 1;
            cmdarr[3] = col; cmdarr[4] = row;
        }
        else {
            if (parse_cell() || *gp++ != ':')
                return 1;
            cmdarr[3] = col; cmdarr[4] = row;
            if (parse_cell() || *gp++ != ')')
                return 1;
            cmdarr[5] = col; cmdarr[6] = row;

            /* Invalid range checks */
            if ((cmdarr[3] > cmdarr[5]) || (cmdarr[4] > cmdarr[6]))
                return 1;
        }

        if (*gp == '\0')
            return 0;
        else
            return 1;
    }
}

int parse_cell()
{
    col = row = 0;

    /* Cell parsing */
    while (*gp >= 'A' && *gp <= 'Z') {
        if (col <= (INT_MAX - (*gp - 'A' + 1)) / 26)
            col = col*26 + (*gp - 'A' + 1);
        else
            col = INT_MAX;
        gp++;
    }
    if (*gp == '0') return 1;
    while (*gp >= '0' && *gp <= '9') {
        if (row <= (INT_MAX - (*gp - '0')) / 10)
            row = row*10 + (*gp - '0');
        else
            row = INT_MAX;
        gp++;
    }
    
    return ((col <= 0) || (col > ncols) || (row <= 0) || (row > nrows));
}

int parse_int()
{
    int sign = 0;
    col = row = 0;

    /* Int parsing */
    if (*gp == '+' || *gp == '-') {
        sign = (*gp == '-');
        gp++;
    }

    if (sign) {
        while (*gp >= '0' && *gp <= '9') {
            if (row >= (INT_MIN + (*gp - '0')) / 10)
                row = row*10 - (*gp - '0');
            else
                row = INT_MIN;
            gp++;
        }            
    }
    else {
        while (*gp >= '0' && *gp <= '9') {
            if (row <= (INT_MAX - (*gp - '0')) / 10)
                row = row*10 + (*gp - '0');
            else
                row = INT_MAX;
            gp++;
        }
    }

    return (row == 0);
}

int parse_uint()
{
    col = row = 0;

    /* Uint parsing */
    while (*gp >= '0' && *gp <= '9') {
        if (row <= (INT_MAX - (*gp - '0')) / 10)
            row = row*10 + (*gp - '0');
        else
            row = INT_MAX;
        gp++;
    }

    return (row == 0);
}

int process_cmdarr()
{
    if (cmdarr[0] <= 0) return 0;
    col = cmdarr[0] - 1; row = cmdarr[1] - 1;
    int col1 = cmdarr[3] - 1, row1 = cmdarr[4] - 1;
    int col2 = cmdarr[5] - 1, row2 = cmdarr[6] - 1;
    int i = col + row * ncols;
    nalloc(col, row);

    /* Simple assignment expression */
    if (cmdarr[2] == 0) {
        if (col1 == 0)
            updateNode(lookup[i], NULL, 0, '+', row1);
        else {
            nalloc(col1, row1);
            struct Node **dep_upon = (struct Node **)malloc(sizeof(struct Node *));
            dep_upon[0] = lookup[col1 + row1 * ncols];
            updateNode(lookup[i], dep_upon, 1, '+', 0);
        }
    }
    /* Arithmetic expression */
    else if (cmdarr[2] >= 1 && cmdarr[2] <= 4) {
        if (col1 == 0 && col2 == 0) {
            switch (cmdarr[2]) {
                case 1:
                    updateNode(lookup[i], NULL, 0, '+', row1 + row2);
                    break;
                case 2:
                    updateNode(lookup[i], NULL, 0, '-', row1 - row2);
                    break;
                case 3:
                    updateNode(lookup[i], NULL, 0, '*', row1 * row2);
                    break;
                case 4:
                    if (row2 == 0) {
                        lookup[i]->is_err = 1;
                        updateNode(lookup[i], NULL, 0, '/', 0);
                    }
                    else
                        updateNode(lookup[i], NULL, 0, '/', row1 / row2);
                    break;
            }
        }
        else if (col1 == 0) {
            nalloc(col2, row2);
            struct Node **dep_upon = (struct Node **)malloc(2 * sizeof(struct Node *));
            dep_upon[0] = NULL; dep_upon[1] = lookup[col2 + row2 * ncols];
            switch (cmdarr[2]) {
                case 1:
                    updateNode(lookup[i], dep_upon, 1, '+', row1);
                    break;
                case 2:
                    updateNode(lookup[i], dep_upon, 1, '-', row1);
                    break;
                case 3:
                    updateNode(lookup[i], dep_upon, 1, '*', row1);
                    break;
                case 4:
                    updateNode(lookup[i], dep_upon, 1, '/', row1);
                    break;
            }
        }
        else if (col2 == 0) {
            nalloc(col1, row1);
            struct Node **dep_upon = (struct Node **)malloc(sizeof(struct Node *));
            dep_upon[0] = lookup[col1 + row1 * ncols];
            switch (cmdarr[2]) {
                case 1:
                    updateNode(lookup[i], dep_upon, 1, '+', row2);
                    break;
                case 2:
                    updateNode(lookup[i], dep_upon, 1, '-', row2);
                    break;
                case 3:
                    updateNode(lookup[i], dep_upon, 1, '*', row2);
                    break;
                case 4:
                    updateNode(lookup[i], dep_upon, 1, '/', row2);
                    break;
            }
        }
        else {
            nalloc(col1, row1); nalloc(col2, row2);
            struct Node **dep_upon = (struct Node **)malloc(2 * sizeof(struct Node *));
            dep_upon[0] = lookup[col1 + row1 * ncols]; dep_upon[1] = lookup[col2 + row2 * ncols];
            switch (cmdarr[2]) {
                case 1:
                    updateNode(lookup[i], dep_upon, 2, '+', 0);
                    break;
                case 2:
                    updateNode(lookup[i], dep_upon, 2, '-', 0);
                    break;
                case 3:
                    updateNode(lookup[i], dep_upon, 2, '*', 0);
                    break;
                case 4:
                    updateNode(lookup[i], dep_upon, 2, '/', 0);
                    break;
            }
        }
    }
    /* Function */
    else {
        if (cmdarr[2] == 10) {
            if (col1 == 0)
                updateNode(lookup[i], NULL, 0, 'l', row1);
            else {
                nalloc(col1, row1);
                struct Node **dep_upon = (struct Node **)malloc(sizeof(struct Node *));
                dep_upon[0] = lookup[col1 + row1 * ncols];
                updateNode(lookup[i], dep_upon, 1, 'l', 0);
            }
        }
        else {
            int r = (row2 - row1); int c = (col2 - col1);
            struct Node **dep_upon = (struct Node **)malloc((r + 1) * (c + 1) * sizeof(struct Node *));
            for (int j = row1; j <= row2; j++)
                for (int k = col1; k <= col2; k++) {
                    nalloc(k, j);
                    dep_upon[j * c + k] = lookup[j * ncols + k];
                }
            switch (cmdarr[2]) {
                case 5:
                    updateNode(lookup[i], dep_upon, (r + 1) * (c + 1), 'm', 0);
                    break;
                case 6:
                    updateNode(lookup[i], dep_upon, (r + 1) * (c + 1), 'M', 0);
                    break;
                case 7:
                    updateNode(lookup[i], dep_upon, (r + 1) * (c + 1), 'a', 0);
                    break;
                case 8:
                    updateNode(lookup[i], dep_upon, (r + 1) * (c + 1), 's', 0);
                    break;
                case 9:
                    updateNode(lookup[i], dep_upon, (r + 1) * (c + 1), 'S', 0);
                    break;
            }
        }
    }

    return 0;
}

void nalloc(int col, int row) {
    int i = col + row * ncols;
    if (lookup[i] != NULL) return;

    char *name = (char *)malloc(7 * sizeof(char));
    char colname[4]; int j = 0, k = 0;

    while (col >= 0) {
        colname[j++] = 'A' + (col % 26);
        col = (col / 26) - 1;
    }
    
    for (k = 0; k < j / 2; k++) {
        char temp = colname[k];
        colname[k] = colname[j - k - 1];
        colname[j - k - 1] = temp;
    }
    colname[j] = '\0';

    sprintf(name, "%s%d", colname, row + 1);

    lookup[i] = (struct Node *)malloc(sizeof(struct Node));
    lookup[i]->name = name;
    lookup[i]->value = 0;
    lookup[i]->dependencies = NULL;
    lookup[i]->depCount = 0;
    lookup[i]->dependent_upon = NULL;
    lookup[i]->dCount = 0;
    lookup[i]->opcode = '+';
    lookup[i]->constant = 0;
    lookup[i]->visited = 0;
    lookup[i]->is_err = 0;
    
    return;
}
