/* Scanner + Parser */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Variables */
int print_sheet = 1;
float exec_time = 0.0;
char *exec_status = "ok";
char *cmd;
char *gp;
char *lp;
int cmdarr[7] = {0, 0, 0, 0, 0, 0, 0};
int col = 0;
int row = 0;

/* Constants */
char *OK = "ok";
char *ERR1 = "unrecognized cmd";
char *ERR2 = "invalid cell";
char *ERR3 = "invalid range";

/* Spreadsheet params */
int nrows = 999;
int ncols = 18278;

/* Prototypes */
int parser(void);
int parser_aux(void);

int main()
{
    char c; int len; int cap;
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

        /* Initialize */
        for (int i = 0; i < 7; i++)
            cmdarr[i] = 0;

        /* Parse input */
        printf("%s\n", cmd);
        c = parser();
        printf("cmdarr[]:");
        for (int i = 0; i < 7; i++)
            printf(" %d", cmdarr[i]);
        printf("\n%d\n", c);
        free(cmd);

        /* Process return status of parser */
        switch(c) {
            case 0:
                exec_status = OK;
                break;
            case 1:
                exec_status = ERR1;
                break;
            case 2:
                exec_status = ERR2;
                break;
            case 3:
                exec_status = ERR3;
                break;
        }

        /* Quit */
        if (cmdarr[0] == -1)
            break;
    }
}

int parser()
{
    gp = cmd;
    char temp; char *temp1; char *temp2;

    /* Cmd parsing */
    while (*gp == ' ' || *gp == '\t')
        gp++;
    temp = *(gp + 1); *(gp + 1) = '\0';
    if (strcmp(gp, "q") == 0)
        cmdarr[0] = -1;
    else if (strcmp(gp, "w") == 0)
        cmdarr[0] = 1;
    else if (strcmp(gp, "d") == 0)
        cmdarr[0] = 2;
    else if (strcmp(gp, "a") == 0)
        cmdarr[0] = 3;
    else if (strcmp(gp, "s") == 0)
        cmdarr[0] = 4;
    gp += 1; *gp = temp;
    while (*gp == ' ' || *gp == '\t')
        gp++;
    if (cmdarr[0] != 0) {
        if (*gp == '\0')
            return 0;
        else
            return 1;
    }
    
    gp -= 1;
    /* Cell parsing */
    if (parser_aux()) return 2;
    if (col == 0 || row == 0 || *gp++ != '=')
            return 1;
    cmdarr[0] = col; cmdarr[1] = row;

    /* Expression parsing */
    temp1 = gp; temp2 = lp;
    if (parser_aux()) return 2;
    if (row != 0) {
        cmdarr[3] = col; cmdarr[4] = row;
        /* Assignment expression */
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
        if (parser_aux()) return 2;
        if (row == 0 || *gp++ != '\0')
            return 1;
        cmdarr[5] = col; cmdarr[6] = row;
    }
    /* Function */
    else if (col != 0 && *gp == '(' ) {
        *temp2 = '\0';
        if (strcmp(temp1, "MIN") == 0)
            cmdarr[2] = 5;
        else if (strcmp(temp1, "MAX") == 0)
            cmdarr[2] = 6;
        else if (strcmp(temp1, "AVG") == 0)
            cmdarr[2] = 7;
        else if (strcmp(temp1, "SUM") == 0)
            cmdarr[2] = 8;
        else if (strcmp(temp1, "STDEV") == 0)
            cmdarr[2] = 9;
        else if (strcmp(temp1, "SLEEP") == 0)
            cmdarr[2] = 10;
        else
            return 1;
        if (cmdarr[2] == 10) {
            if (parser_aux()) return 2;
            if (row == 0 || *gp++ != ')')
                return 1;
            cmdarr[3] = col; cmdarr[4] = row;
        }
        else {
            if (parser_aux()) return 2;
            if (col == 0 || row == 0 || *gp++ != ':')
                return 1;
            cmdarr[3] = col; cmdarr[4] = row;
            if (parser_aux()) return 2;
            if (col == 0 || row == 0 || *gp++ != ')')
                return 1;
            cmdarr[5] = col; cmdarr[6] = row;

            /* Invalid range checks */
            if ((cmdarr[3] > cmdarr[5]) || (cmdarr[4] > cmdarr[6]))
                return 3;
        }
        while (*gp == ' ' || *gp == '\t')
            gp++;
        if (*gp == '\0')
            return 0;
        else
            return 1;
    }
    else
        return 1;
}

int parser_aux()
{
    col = row = 0;

    /* Cell type parsing */
    while (*gp == ' ' || *gp == '\t')
        gp++;
    while (*gp >= 65 && *gp <= 90 && col <= ncols)
        if (col <= ncols)
            col = col*26 + (*gp++ - 64);
    lp = gp;
    while (*gp >= 48 && *gp <= 57 && row <= nrows)
        if (row <= nrows)
            row = row*10 + (*gp++ - 48);
    while (*gp == ' ' || *gp == '\t')
        gp++;

    /* Invalid cell flags*/
    if ((col > ncols) || (row > nrows) || ((row == 0) && (lp != gp)))
        return 1;
    
    return 0;
}
