#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");
    if (!f1 || !f2) {
        perror("Error opening files");
        return 1;
    }

    char line1[512], line2[512]; // we can change the array length later
    while (fgets(line1, sizeof(line1), f1) && fgets(line2, sizeof(line2), f2)) {
        if (strstr(line1, "[") && strstr(line2, "[")) {
            char *time1 = strchr(line1, '[');
            char *time2 = strchr(line2, '[');
            time1++;
            time2++;
            while(*time1 != ']'){               // repalacing all the [time] with [000]                                               // 
                *time1 = '0';                   // time need not be compared
                time1++;                        // but i have taken the assumption that
            }                                   // both expected and output contains time string
            while(*time2 != ']') {              // having same length 
                *time2 = '0';
                time2++;
            }
        }
        if (strcmp(line1, line2) != 0) {
            fclose(f1);
            fclose(f2);
            return 1;
        }
    }

    fclose(f1);
    fclose(f2);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Use this format: %s <output_file> <expected_output_file>\n", argv[0]);
        return 1;
    }
    if (compare_files(argv[1], argv[2]) == 0) {
        printf("Output matches expected output.\n");
    } else {
        printf("Output does not match expected output.\n");
    }

    return 0;
}
