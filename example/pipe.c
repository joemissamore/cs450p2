#include<stdio.h> 
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<errno.h>
#include<string.h>

int main() {

    int pfd[2];
    pipe(pfd);

    char *words[2] = {"ls"};
    char *wordss[3] = {"grep", "file"};

    if (fork() == 0) {
        close(0);
        dup2(pfd[1], 1);
        execvp(words[0], words);
    }

    if (fork() == 0) {
        dup2(pfd[0], 0);
        execvp(wordss[0], wordss);
    }

    return 0;
}