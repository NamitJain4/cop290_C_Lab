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
int cmdarr[7] = {0, 0, 0, 0, 0, 0, 0};
int col = 0;
int row = 0;

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

int main(int argc, char *argv[])
{
    if (argc != 3) return -1;
    gp = argv[1];
    if (parse_uint() || *gp || (nrows = row) > 999) return -1;
    gp = argv[2];
    if (parse_uint() || *gp || (ncols = row) > 18278) return -1;

    char c; int len; int cap;
    while (1) {
        if (print_sheet && cmdarr[0] != -7) ; // call print_sheet
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
                exec_status = ERR;
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
    int status = 1; char *temp;

    /* Cmd test */
    if ((status = strcmp(gp, "q")) == 0)
        cmdarr[0] = -1;
    else if ((status = strcmp(gp, "w")) == 0)
        cmdarr[0] = -2;
    else if ((status = strcmp(gp, "d")) == 0)
        cmdarr[0] = -3;
    else if ((status = strcmp(gp, "a")) == 0)
        cmdarr[0] = -4;
    else if ((status = strcmp(gp, "s")) == 0)
        cmdarr[0] = -5;
    else if ((status = strcmp(gp, "disable_output")) == 0) {
        cmdarr[0] = -6; print_sheet = 0;
    }
    else if ((status = strcmp(gp, "enable_output")) == 0) {
        cmdarr[0] = -7; print_sheet = 1;
    }
    if (status == 0)
        return 0;

    if (strncmp(gp, "scroll_to ", 10) == 0) {
        gp += 10;
        if (parse_cell() || *gp++ != '\0')
            return 1;
        cmdarr[0] = -8; cmdarr[1] = col; cmdarr[2] = row;
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
            if (parse_uint() || *gp++ != ')')
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
    while (*gp >= 65 && *gp <= 90) {
        if (col <= (INT_MAX - (*gp - 64)) / 26)
            col = col*26 + (*gp - 64);
        else
            col = INT_MAX;
        gp++;
    }
    if (*gp == 48) return 1;
    while (*gp >= 48 && *gp <= 57) {
        if (row <= (INT_MAX - (*gp - 48)) / 10)
            row = row*10 + (*gp - 48);
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
        while (*gp >= 48 && *gp <= 57) {
            if (row >= (INT_MIN + (*gp - 48)) / 10)
                row = row*10 - (*gp - 48);
            else
                row = INT_MIN;
            gp++;
        }            
    }
    else {
        while (*gp >= 48 && *gp <= 57) {
            if (row <= (INT_MAX - (*gp - 48)) / 10)
                row = row*10 + (*gp - 48);
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
    while (*gp >= 48 && *gp <= 57) {
        if (row <= (INT_MAX - (*gp - 48)) / 10)
            row = row*10 + (*gp - 48);
        else
            row = INT_MAX;
        gp++;
    }

    return (row == 0);
}
