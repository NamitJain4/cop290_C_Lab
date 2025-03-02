/* Scanner + Parser */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* Variables */
int flag = 0;
float exec_time = 0.0;
char *exec_status = "ok";
char *cmd;
char *gp;
int cmdarr[7] = {0, 0, 0, 0, 0, 0, 0};
int testarr[7] = {0, 0, 0, 0, 0, 0, 0};
int col = 0;
int row = 0;

/* Constants */
char *OK = "ok";
char *ERR = "unrecognized cmd";

/* Spreadsheet params */
int nrows;
int ncols;

/* Prototypes */
int parser(void);
int parse_cell(void);
int parse_int(void);
int parse_uint(void);
void test(void);

int main()
{
    printf("Runnning Testcases for Parser Implementation:\n");

    printf("\nSimple Commands:\n");
    cmd = "q"; printf("%-20s: ", cmd);
    testarr[0] = -1; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "w"; printf("%-20s: ", cmd);
    testarr[0] = -2; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "d"; printf("%-20s: ", cmd);
    testarr[0] = -3; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "a"; printf("%-20s: ", cmd);
    testarr[0] = -4; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "s"; printf("%-20s: ", cmd);
    testarr[0] = -5; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "disable_output"; printf("%-20s: ", cmd);
    testarr[0] = -6; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "enable_output"; printf("%-20s: ", cmd);
    testarr[0] = -7; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "scroll_to ZZZ999"; printf("%-20s: ", cmd);
    testarr[0] = -8; testarr[1] = 18278; testarr[2] = 999; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();

    printf("\nSimple Assignemnt Expressions:\n");
    cmd = "A1=2"; printf("%-20s: ", cmd);
    testarr[0] = 1; testarr[1] = 1; testarr[2] = 0; testarr[3] = 0; testarr[4] = 2; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "A1=B1"; printf("%-20s: ", cmd);
    testarr[0] = 1; testarr[1] = 1; testarr[2] = 0; testarr[3] = 2; testarr[4] = 1; testarr[5] = 0; testarr[6] = 0;
    test();

    printf("\nArithmetic Expressions:\n");
    cmd = "A1=B2+C3"; printf("%-20s: ", cmd);  
    testarr[0] = 1; testarr[1] = 1; testarr[2] = 1; testarr[3] = 2; testarr[4] = 2; testarr[5] = 3; testarr[6] = 3;
    test();  
    cmd = "ZZ10=AA5-B7"; printf("%-20s: ", cmd);  
    testarr[0] = 702; testarr[1] = 10; testarr[2] = 2; testarr[3] = 27; testarr[4] = 5; testarr[5] = 2; testarr[6] = 7;
    test();  
    cmd = "C5=4*D6"; printf("%-20s: ", cmd);  
    testarr[0] = 3; testarr[1] = 5; testarr[2] = 3; testarr[3] = 0; testarr[4] = 4; testarr[5] = 4; testarr[6] = 6;
    test();  
    cmd = "E8=G9/2"; printf("%-20s: ", cmd);  
    testarr[0] = 5; testarr[1] = 8; testarr[2] = 4; testarr[3] = 7; testarr[4] = 9; testarr[5] = 0; testarr[6] = 2;
    test();  
    cmd = "M10=X2/0"; printf("%-20s: ", cmd);  
    testarr[0] = 13; testarr[1] = 10; testarr[2] = 4; testarr[3] = 24; testarr[4] = 2; testarr[5] = 0; testarr[6] = 0;
    test();  
    cmd = "D4=-3+F5"; printf("%-20s: ", cmd);  
    testarr[0] = 4; testarr[1] = 4; testarr[2] = 1; testarr[3] = 0; testarr[4] = -3; testarr[5] = 6; testarr[6] = 5;
    test();  
    cmd = "ZZ1000=Y999*X888"; printf("%-20s: ", cmd);  
    testarr[0] = 702; testarr[1] = 1000; testarr[2] = 3; testarr[3] = 25; testarr[4] = 999; testarr[5] = 24; testarr[6] = 888;
    test();

    printf("\nFunction Expressions:\n");
    cmd = "A1=MIN(A1:Z1)"; printf("%-20s: ", cmd);  
    testarr[0] = 1; testarr[1] = 1; testarr[2] = 5; testarr[3] = 1; testarr[4] = 1; testarr[5] = 26; testarr[6] = 1;
    test();
    cmd = "B2=MAX(A1:Z10)"; printf("%-20s: ", cmd);  
    testarr[0] = 2; testarr[1] = 2; testarr[2] = 6; testarr[3] = 1; testarr[4] = 1; testarr[5] = 26; testarr[6] = 10;
    test();  
    cmd = "C3=AVG(D4:M12)"; printf("%-20s: ", cmd);  
    testarr[0] = 3; testarr[1] = 3; testarr[2] = 7; testarr[3] = 4; testarr[4] = 4; testarr[5] = 13; testarr[6] = 12;
    test();
    cmd = "D4=SUM(A1:G5)"; printf("%-20s: ", cmd);  
    testarr[0] = 4; testarr[1] = 4; testarr[2] = 8; testarr[3] = 1; testarr[4] = 1; testarr[5] = 7; testarr[6] = 5;
    test();  
    cmd = "E5=STDEV(K3:T8)"; printf("%-20s: ", cmd);  
    testarr[0] = 5; testarr[1] = 5; testarr[2] = 9; testarr[3] = 11; testarr[4] = 3; testarr[5] = 20; testarr[6] = 8;
    test();  
    cmd = "F6=SLEEP(-6)"; printf("%-20s: ", cmd);
    testarr[0] = 6; testarr[1] = 6; testarr[2] = 10; testarr[3] = 0; testarr[4] = -6; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "G7=SLEEP(E3)"; printf("%-20s: ", cmd);
    testarr[0] = 7; testarr[1] = 7; testarr[2] = 10; testarr[3] = 5; testarr[4] = 3; testarr[5] = 0; testarr[6] = 0;
    test();

    printf("\nInvalid Commands:\n");
    cmd = "A1=hello"; printf("%-20s: ", cmd);  
    testarr[0] = 0; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "A1=B0+C3"; printf("%-20s: ", cmd);  
    testarr[0] = 0; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "A01=6"; printf("%-20s: ", cmd);  
    testarr[0] = 0; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();
    cmd = "A1=MAX(D6:A1)"; printf("%-20s: ", cmd);  
    testarr[0] = 0; testarr[1] = 0; testarr[2] = 0; testarr[3] = 0; testarr[4] = 0; testarr[5] = 0; testarr[6] = 0;
    test();

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
        cmdarr[0] = -2;
    else if ((status = strcmp(gp, "d")) == 0)
        cmdarr[0] = -3;
    else if ((status = strcmp(gp, "a")) == 0)
        cmdarr[0] = -4;
    else if ((status = strcmp(gp, "s")) == 0)
        cmdarr[0] = -5;
    else if ((status = strcmp(gp, "disable_output")) == 0)
        cmdarr[0] = -6;
    else if ((status = strcmp(gp, "enable_output")) == 0)
        cmdarr[0] = -7;
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
    int sign = 0; char *temp;
    col = row = 0;

    /* Int parsing */
    if (*gp == '+' || *gp == '-') {
        sign = (*gp == '-');
        gp++;
    }

    temp = gp;
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

    return (row == 0 && gp == temp);
}

int parse_uint()
{
    char *temp;
    col = row = 0;

    /* Uint parsing */
    while (*gp >= '0' && *gp <= '9') {
        if (row <= (INT_MAX - (*gp - '0')) / 10)
            row = row*10 + (*gp - '0');
        else
            row = INT_MAX;
        gp++;
    }

    return (row == 0 && gp == temp);
}

void test() {
    int i, c;

    /* Initialize */
    for (i = 0; i < 7; i++)
        cmdarr[i] = 0;
    
    c = parser();

    /* Process return status of parser */
    switch(c) {
        case 0:
            exec_status = OK;
            break;
        case 1:
            exec_status = ERR;
            break;
    }

    int b = 1;

    if (c == 0) {
        for (i = 0; i < 7; i++)
            b = b && (cmdarr[i] == testarr[i]);
        b = b && (strcmp(exec_status, OK) == 0);
    }
    else
        b = b && (strcmp(exec_status, ERR) == 0);

    if (b)
        printf("Passed\n");
    else
        printf("Failed\n");

    return;
}
