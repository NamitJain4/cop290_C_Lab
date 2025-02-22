/* Scanner + Parser */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int print_sheet = 1;
float exec_time = 0.0;
char *exec_status = "ok";
char *err = "unrecognized cmd";
char *cmd = "\0";

int main()
{
    while (strcmp(cmd, "q")) {
        if (print_sheet) {
        printf("[%.1f] (%s) > ", exec_time, exec_status);
        scanf("%s", cmd);
        }
    }
}

void scan()
{
    return;
}
