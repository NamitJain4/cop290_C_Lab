#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

int rows = 100; 
int cols = 200; 
int sheet[100][200]; 
int print_arr[9];

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
                temp = MAX((rows-9), 1);
                if ((row_multiple+10) > rows){
                    break;
                }
                row_multiple = MIN(row_multiple + 10, temp);
                break;
            case (int)'d':
                temp = MAX((cols-9), 1);
                if ((col_multiple+10) > cols){
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

    int max_col = MIN(col_multiple + 8, cols - 1);
    int max_row = MIN(row_multiple + 8, rows - 1);
    int min_col = MAX(col_multiple - 1, 0);
    int min_row = MAX(row_multiple - 1, 0);

    for (int i = min_col; i <= max_col; i++) {
        int length = 0;
        int k = i;
        while (k > 0) {
            k = k / 26;
            length++;
        }
        length = MAX(1, length) + 1;
        char *s = (char *)malloc(length * sizeof(char));
        length--;
        s[length] = '\0';
        k = i;
        while (length--) {
            s[length] = (char)('A' + (k % 26));
            k /= 26;
        }
        printf("\t%s", s);
        free(s);
    }
    for (int i = min_row; i <= max_row; i++) {
        printf("\n");
        printf("%d", (i + 1));
        for (int j = min_col; j <= max_col; j++) {
            printf("\t%d", sheet[i][j]);
        }
    }
    printf("\n");
}

void initializeSheet() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            sheet[i][j] = j+1;
        }
    }
}

void initializePrintArr() {
    for (int i = 0; i < 9; i++) {
        print_arr[i] = 0;
    }
    print_arr[7] = 1; // Initial top left row
    print_arr[8] = 1; // Initial top left col
}

int tests_passed = 0;
int total_tests = 0;

void test_enable_output() {
    print_arr[0] = 0;
    printSheet();
    assert(print_arr[0] == 0);
    tests_passed++;
    total_tests++;

    print_arr[0] = 1;
    printSheet();
    assert(print_arr[0] == 1);
    tests_passed++;
    total_tests++;

    print_arr[0] = 1;
    printSheet();
    assert(print_arr[0] == 1);
    tests_passed++;
    total_tests++;

    printf("Test enable_output passed.\n");
}

void test_disable_output() {
    print_arr[0] = 1;
    printSheet();
    assert(print_arr[0] == 1);
    tests_passed++;
    total_tests++;

    print_arr[0] = 0;
    printSheet();
    assert(print_arr[0] == 0);
    tests_passed++;
    total_tests++;

    print_arr[0] = 0;
    printSheet();
    assert(print_arr[0] == 0);
    tests_passed++;
    total_tests++;

    printf("Test disable_output passed.\n");
}

void test_scroll_to() {
    print_arr[3] = 5;
    print_arr[4] = 5;
    print_arr[1] = 1;
    printSheet();
    assert(print_arr[7] == 5);
    assert(print_arr[8] == 5);
    tests_passed++;
    total_tests++;

    print_arr[3] = 10;
    print_arr[4] = 10;
    print_arr[1] = 1;
    printSheet();
    assert(print_arr[7] == 10);
    assert(print_arr[8] == 10);
    tests_passed++;
    total_tests++;

    print_arr[3] = 15;
    print_arr[4] = 15;
    print_arr[1] = 1;
    printSheet();
    assert(print_arr[7] == 15);
    assert(print_arr[8] == 15);
    tests_passed++;
    total_tests++;

    printf("Test scroll_to passed.\n");
}

void test_scroll_w() {
    print_arr[5] = 1;
    print_arr[6] = 'w';
    printSheet();
    assert(print_arr[7] == 5);
    tests_passed++;
    total_tests++;

    print_arr[7] = 11;
    print_arr[5] = 1;
    print_arr[6] = 'w';
    printSheet();
    assert(print_arr[7] == 1);
    tests_passed++;
    total_tests++;

    print_arr[7] = 21;
    print_arr[5] = 1;
    print_arr[6] = 'w';
    printSheet();
    assert(print_arr[7] == 11);
    tests_passed++;
    total_tests++;

    printf("Test scroll_w passed.\n");
}

void test_scroll_a() {
    print_arr[5] = 1;
    print_arr[6] = 'a';
    printSheet();
    assert(print_arr[8] == 1);
    tests_passed++;
    total_tests++;

    print_arr[8] = 11;
    print_arr[5] = 1;
    print_arr[6] = 'a';
    printSheet();
    assert(print_arr[8] == 1);
    tests_passed++;
    total_tests++;

    print_arr[8] = 21;
    print_arr[5] = 1;
    print_arr[6] = 'a';
    printSheet();
    assert(print_arr[8] == 11);
    tests_passed++;
    total_tests++;

    printf("Test scroll_a passed.\n");
}

void test_scroll_s() {
    print_arr[5] = 1;
    print_arr[6] = 's';
    printSheet();
    assert(print_arr[7] == 11);
    tests_passed++;
    total_tests++;

    print_arr[5] = 1;
    print_arr[6] = 's';
    printSheet();
    assert(print_arr[7] == 21);
    tests_passed++;
    total_tests++;

    print_arr[5] = 1;
    print_arr[6] = 's';
    printSheet();
    assert(print_arr[7] == 31);
    tests_passed++;
    total_tests++;

    printf("Test scroll_s passed.\n");
}

void test_scroll_d() {
    print_arr[5] = 1;
    print_arr[6] = 'd';
    printSheet();
    assert(print_arr[8] == 11);
    tests_passed++;
    total_tests++;

    print_arr[5] = 1;
    print_arr[6] = 'd';
    printSheet();
    assert(print_arr[8] == 21);
    tests_passed++;
    total_tests++;

    print_arr[5] = 1;
    print_arr[6] = 'd';
    printSheet();
    assert(print_arr[8] == 31);
    tests_passed++;
    total_tests++;

    printf("Test scroll_d passed.\n");
}

int main() {
    initializeSheet();
    initializePrintArr();

    test_enable_output();
    test_disable_output();
    test_scroll_to();
    test_scroll_w();
    print_arr[7] = 1;
    print_arr[8] = 1;
    test_scroll_a();
    print_arr[7] = 1;
    print_arr[8] = 1;
    test_scroll_s();
    print_arr[7] = 1;
    print_arr[8] = 1;
    test_scroll_d();

    printf("All tests passed: %d/%d\n", tests_passed, total_tests);
    if (tests_passed == total_tests) {
        printf("All tests passed.\n");
    } else {
        printf("Some tests failed.\n");
    }

    return 0;
}
