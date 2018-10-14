#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void strcpyarray(char * dest[], char * source[], int len) {
    for (int i = 0; i < len; i++) {
        dest[i] = (char *) malloc(strlen(source[i]) + 1);
        strcpy(dest[i], source[i]);
    }
}

void slicearray(char * dest[], char * source[], int from, int to) {
    for (int i = from, j =0; i <= to; i++, j++) {
        dest[j] = (char *) malloc(strlen(source[i]) + 1);
        strcpy(dest[j], source[i]);
    }
}

void printArray(char * c[], int len) {
    for (int i = 0; i < len; i++) {
        printf("%s\n", c[i]);
    }
}

int main() {
    
    int i = 0;
    printf("%d\n", i++);
    printf("%d\n", ++i);


    // Copying strings
    char * c[2] = {"Hello1", "String2"};
    char * dest[2];
    printArray(c, 2);
    strcpyarray(dest, c, 2);
    c[0] = "";
    c[1] = "";
    printArray(dest, 2);
    char * slice[1];
    slicearray(slice, dest, 1, 1);
    printArray(slice , 1);
    int num_args = 2;
    char * c1[num_args];

    


    return 0;
}