#include "common.h"

extern struct Node **lookup;

/*
for print_arr -----------------------------------------------------------------
0th index - disable(0 or 1)
1st index - scroll_to_cell signal(0 or 1) (change it to 1 if if cmd is scroll)
            will automatically change to 0 once scrolled
2nd index - (empty)
3rd index - scroll to cell's row (1..)
4th index - scroll to cell's col (1..)
5th index - scroll by (10 col or row) signal((change it to 1 if if cmd is scroll)
            will automatically change to 0 once scrolled)(i.e. when cmd is w,a,s,d)
6th index - char of above cmd (w, a, s or d)
7th index - current top left row no. of cell (need not be changed externally)
8th index - current top left col no. of cell (need not be changed externally)
*/

void printSheet () {
    int row_multiple = print_arr[7];
    int col_multiple = print_arr[8];

    if (print_arr[1] == 1){
        row_multiple = print_arr[3];
        col_multiple = print_arr[4];
        print_arr[1] = 0;
    } 
    else if (print_arr[5] == 1){
        int temp;
        switch (print_arr[6]){
            case (int)'w':
                row_multiple = MAX(row_multiple - 10, 1);
                break;
            case (int)'a':
                col_multiple = MAX(col_multiple - 10, 1);
                break;
            case (int)'s':
                temp = MAX((nrows-9), 1);
                if ((row_multiple+10) > nrows){
                    break;
                }
                row_multiple = MIN(row_multiple + 10, temp);
                break;
            case (int)'d':
                temp = MAX((ncols-9), 1);
                if ((col_multiple+10) > ncols){
                    break;
                }
                col_multiple = MIN(col_multiple + 10, temp);
                break;
        }
        print_arr[5] = 0;
    }

    print_arr[7] = row_multiple;
    print_arr[8] = col_multiple;

    if (print_arr[0] == 1) {
        return;
    }

    int max_col = MIN(col_multiple + 8, ncols - 1);
    int max_row = MIN(row_multiple + 8, nrows - 1);
    int min_col = MAX(col_multiple - 1, 0);
    int min_row = MAX(row_multiple - 1, 0);

    for (int i = min_col; i <= max_col; i++) {
        int length = 0;
        int k = i;
        while (k >= 0) {
            k = (k / 26) - 1;
            length++;
        }
        length = MAX(1, length) + 1;
        char *s = (char *)malloc(length * sizeof(char));
        length--;
        s[length] = '\0';
        k = i;
        while (length--) {
            s[length] = (char)('A' + (k % 26));
            k = (k / 26) - 1;
        }
        printf("\t%s", s);
        free(s);
    }
    for (int i = min_row; i <= max_row; i++) {
        printf("\n");
        printf("%d", (i + 1));
        for (int j = min_col; j <= max_col; j++) {
            int index = i*ncols + j;
            if (lookup[index] == NULL){
                printf("\t%d", 0);
            }
            else{
                if (lookup[index]->is_err){
                    printf("\tERR");
                }
                else{
                    printf("\t%d", lookup[index]->value);
                }
            }
        }
    }
    printf("\n");
}
