#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* For open() */
#include <fcntl.h>
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>
/* end open() */

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
    
    // int i = 0;
    // printf("%d\n", i++);
    // printf("%d\n", ++i);


    // // Copying strings
    // char * c[2] = {"Hello1", "String2"};
    // char * dest[2];
    // printArray(c, 2);
    // strcpyarray(dest, c, 2);
    // c[0] = "";
    // c[1] = "";
    // printArray(dest, 2);
    // char * slice[1];
    // slicearray(slice, dest, 1, 1);
    // printArray(slice , 1);
    // int num_args = 2;
    // char * c1[num_args];

    

    // // execlp
    //  int pfds[10][2];

    // for (int x = 0; x < 10; x++) {
    //     pipe(pfds[x]);
    // }

    // // sleep(60);
    // if (fork() == 0) {
    //     dup2(pfds[0][1], 1);
    //     for (int i = 3; i <= 20; i++) {
    //         close(i);
    //     }
    //     // execlp("ls", "ls", "-l", "-a", "-x", NULL);
    //     char * c[] = {"ls", "-l", "-a", "-x", NULL};
    //     execvp(c[0], c);
    // }

    // // if (fork() == 0) {
    // //     dup2(pfds[0][0], 0);
    // //     dup2(pfds[1][1], 1);
    // //     for (int i = 3; i <= 20; i++) {
    // //         close(i);
    // //     }
    // //     execlp("grep", "grep", "file", NULL);
    // // }

    // if (fork() == 0) {
    //     dup2(pfds[0][0], 0);
    //     for (int i = 3; i <= 20; i++) {
    //         close(i);
    //     }
    //     execlp("wc", "wc", NULL);
    // }


    /*
    
    int bak, new;
    fflush(stdout);
    bak = dup(1);
    new = open("/dev/null", O_WRONLY);
    dup2(new, 1);
    close(new);
    /* your code here ... 
    fflush(stdout);
    dup2(bak, 1);
    close(bak);
    
    */

    // Restoring stdout
    fflush(stdout);
    int saved = dup(1);
    int pfd = open("file.txt", O_WRONLY | O_CREAT, 0777);
    dup2(pfd, 1);
    close(pfd);

    printf("Hi file\n");

    fflush(stdout);
    dup2(saved, 1);
    close(saved);

    return 0;
}