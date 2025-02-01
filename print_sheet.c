#include <stdio.h>
#include <stdlib.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

extern int row_multiple;
extern int col_multiple;
extern int rows;
extern int cols;
extern int sheet[1000][10000];
 
void printSheet (){
    int max_col = MIN(10*col_multiple-1, cols-1);
    int max_row = MIN(10*row_multiple-1, rows-1);
    int min_col = MAX(max_col - 9, 0);
    int min_row = MAX(max_row - 9, 0);

    for(int i = min_col;i <= max_col;i++){
        int length = 0;
        int k = i;
        while(k > 0){
            k = k/26;
            length++;
        }
        length = MAX(1, length) + 1;
        char *s = (char*)malloc(length*sizeof(char));
        length--;
        s[length] = '\0';
        k = i;
        while(length--){
            s[length] = (char)('A' + (k%26));
            k /= 26;
        }
        printf("\t%s", s);
        free(s);
    }
    for(int i = min_row;i <= max_row;i++){
        printf("\n");
        printf("%d", (i + 1));
        for(int j = min_col; j <= (max_col); j++){
            printf("\t%d", sheet[i][j]);
        }
    }
    printf("\n");
} 