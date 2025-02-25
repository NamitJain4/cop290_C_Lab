/* Scanner + Parser */

#include <stdio.h>
#include <stdlib.h>

/* Variables */
int print_sheet = 1;
float exec_time = 0.0;
char *exec_status = "ok";
char *cmd;
char *gp;
int cmdarr[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

/* Constants */
char *OK = "ok";
char *ERR1 = "unrecognized cmd";
char *ERR2 = "invalid cell";
char *ERR3 = "invalid range";

/* Spreadsheet params */
int nrows;
int ncols;

/* Prototypes */
int parser(void);

int main()
{
    char c; char len; char cap;
    while (1) {
        if (print_sheet == 0) continue;
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

        /* Parse input */
        c = parser();
        free(cmd);

        /* Process return status of parser */
        switch(c) {
            case 1:
                exec_status = *ERR1;
                break;
            case 2:
                exec_status = *ERR2;
                break;
        }
    }
}

int parser()
{
    int col = 0; int row = 0;
    gp = cmd;

    /* Cell Parsing */
    while (*gp == ' ' || *gp == '\t')
        gp++;
    while (*gp >= 65 && *gp <= 90 && col <= ncols)
        col = col*26 + (*gp++ - 64);
    while (*gp >= 65 && *gp <= 90)
        gp++;
    char *temp = gp;
    while (*gp >= 48 && *gp <= 57 && row <= nrows)
        row = row*10 + (*gp++ - 48);
    row += (temp != gp);
    while (*gp >= 48 && *gp <= 57)
        gp++;
    while (*gp == ' ' || *gp == '\t')
        gp++;
    if (col == 0 || row == 0 || *gp++ != '=')
            return 1;
    cmdarr[0] = col; cmdarr[1] = row;
    
    /* Expression Parsing */
    col = row = 0;
    while (*gp == ' ' || *gp == '\t')
        gp++;
}
