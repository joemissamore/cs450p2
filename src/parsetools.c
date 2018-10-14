#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "constants.h"
#include "parsetools.h"



// Parse a command line into a list of words,
//    separated by whitespace.
// Returns the number of words
//
int split_cmd_line(char* line, char** list_to_populate) {
   char* saveptr;  // for strtok_r; see http://linux.die.net/man/3/strtok_r
   char* delimiters = " \t\n"; // whitespace
   int i = 0;

    // char *strtok_r(char *str, const char *delim, char **saveptr);
   list_to_populate[0] = strtok_r(line, delimiters, &saveptr);

   while(list_to_populate[i] != NULL && i < MAX_LINE_WORDS - 1)  {
       list_to_populate[++i] = strtok_r(NULL, delimiters, &saveptr);
   };

   return i;
}

bool is_arg_delim(char * w) {
    if (strcmp(w, "|") == 0 ||
            strcmp(w, "<") == 0 ||
            strcmp(w, ">") == 0 || 
            strcmp(w, ">>") == 0) {
                return true;
            }
            return false;
}

void parse(char ** line_words, int num_words) 
{
    // char ** line_words: The entire string of commands

    // number of commands
    // ls -la | grep -a file.txt > output.txt
    // num_commands should = 3
    int num_commands = 0;
    // command length
    // grep -a file.txt
    // should return = 3
    int command_len = 0;

    // array of commands
    struct command commands[100];
    
    int i = 0;
    // Go through all the 'tokens'
    for (; i < num_words; i++) {
        // Capturing the 'token'
        char * w = line_words[i];
        
        // If it sees a redirect we will capture the 
        // entire command. 
        if (is_arg_delim(w)) {
            char ** temp = (char **) malloc(sizeof(command_len + 1));
            slicearray(temp, line_words, i - command_len, i - 1);
            commands[num_commands].command_length = command_len;
            commands[num_commands].command_string = temp;
            commands[num_commands].redirection = w;
       
            num_commands++;
            command_len = 0;
        }
        // if its not a delimiter (<, >, ...) 
        else {
            command_len++;
        }
        
    }
    // i at this point is at the end of the array (out of bounds)
    // needs to be adjusted back to the end of the array (in bounds)
    i--; // need to back i up

    // Allocating space for the double pointer
    // +1 indicates space for the null terminator
    char ** temp = (char **) malloc(sizeof(command_len + 1));
    
    slicearray(temp, line_words, i - command_len + 1, i);
    commands[num_commands].command_string = temp;
    commands[num_commands].command_length = command_len;
    commands[num_commands].redirection = NULL;
    
    // printf("printing the last command...\n");
    // printArray(commands[num_commands].command_string, commands[num_commands].command_length);
    // printf("end printing the last command...\n\n");
    // printf("redirection after: %s\n", commands[num_commands].redirection);
    // printf("i: %d\n", i);
    // printf("command_len: %d\n", command_len);
    // printf("command: \n");
    // printArray(temp, command_len);
    // printf("num_commands: %d\n", num_commands);

    num_commands++;
    // DEBUG
    // 
    // printf("num_commands: %d\n", num_commands);




    if (num_commands == 0) {
        printf("Please enter a command\n");
    }
    else if (num_commands == 1) {
        pid_t pid = fork();
        if (pid == 0)
        {
            _execute(line_words, num_words, -1, -1);
        }
        else if (pid < 0) {
            printf("Error creating child process on command: %s", line_words[0]);
            exit(1);
        }
        
    }


    
    // This is where multiple commands will be executed
    else {
        
        // int num_fds = (num_commands - 1) * 2;

        // int pfd[num_fds]; // pfd[4] ?? 
        // pipe(pfd);
        // TODO: create fds dynamically
        int pfds[10][2];

        for (int x = 0; x < 10; x++) {
            pipe(pfds[x]);
        }
        printf("num_commands: %d\n", num_commands);
        // printf("num_fds: %d\n", num_fds);
        // printf("IN ELSE STATEMENT\n");
        for (int j = 0; j < num_commands; j++) {

            if (j == 0) {
                if (fork() == 0) {
                    dup2(pfds[j / 2][1], 1);
                    execvp(commands[j].command_string[0], commands[j].command_string);
                }
            }
            else if (j == 1) {
                if (fork() == 0) {
                    dup2(pfds[j / 2][0], 0);
                    dup2(pfds[(j + 1) / 2][1], 1);
                    execvp(commands[j].command_string[0], commands[j].command_string);
                }
            } 
            else if (j == 2) {
                if (fork() == 0) {
                    dup2(pfds[j / 2][0], 0);
                    execvp(commands[j].command_string[0], commands[j].command_string);
                }
            }









            // // printf("redirection: %s\n", commands[j].redirection);
            // printf("executing commands: %s\n", commands[j].command_string[0]);

            // if (commands[j].redirection != NULL) {
            //     if (strcmp(commands[j].redirection, "|")== 0) {
            //     printf("pipe\n");
            //     // do for:
            //     // ls -la | grep file
            //         if (fork() == 0) {
            //             printf("fork() == 0\n");
            //             if (j == 0) {
            //                 printf("Inside j==0 (command): %s\n", commands[j].command_string[0]);
            //                 dup2(pfd[1], 1);
            //                 execvp(commands[j].command_string[0], commands[j].command_string);
            //             }
            //             else { 
            //                 printf("Inside else statement (j != 0) (command): %s\n", commands[j].command_string[0]);
            //                 dup2(pfd[j - 1], 0);
            //                 dup2(pfd[j + 1], 1);
            //                 execvp(commands[j].command_string[0], commands[j].command_string);
            //             }
            //         }
            //     } 
            // } 
            // // redirection == NULL
            // else { 
            //     // printf("Inside else statement\n");
            //     printf("redirection == NULL\n");
            //     printf("j: %d\n", j);
            //     if (fork() == 0) {
            //         printf("redirection == NULL (command): %s\n", commands[j].command_string[0]);
            //         dup2(pfd[j], 0); // read from the write
            //         execvp(commands[j].command_string[0], commands[j].command_string);
            //     } 
            //     else {
            //         printf("fork != 0\n");
            //     }
            // }
            
            // printCommandStruct(commands[j]);
    
        //     printf("j: %d", j);

        //     if (j == 0) {
        //         if (fork() == 0) {
        //             _execute(commands[j].command_string, commands[j].command_length, -1, pfd[1]);
        //         }
                
        //     } else {
        //         if (fork() == 0) {
        //             _execute(commands[j].command_string, commands[j].command_length, pfd[j - 1], pfd[j+1]);
        //         }
        //     }
        // }
        // if (fork() == 0) {
        //     _execute(commands[j].command_string, commands[j].command_length, pfd[j - 1], -1);
        }
    }
}


void _execute(char ** line_words, int num_words, int in_pipe, int out_pipe) 
{
    
    if (in_pipe != -1) {
        dup2(in_pipe, 0);
    }
    if (out_pipe != -1) {
        dup2(out_pipe, 1);
    }

    int err_check = execvp(line_words[0], line_words);

    if (err_check == -1) {
        printf("Error with command: %s\n", line_words[0]);
    }
}




void printCommandStruct(struct command com) {
    for (int i = 0; i < com.command_length; i++) {
        printf("%s\n", com.command_string[i]);
    }
    if (com.redirection != NULL) {
        printf("redirection: %s\n", com.redirection);
    }
}

void strcpyarray(char * dest[], char * source[], int len) {
    // If you call this function, it may not work.
    // Was originally following the same logic as slicearray.
    for (int i = 0; i < len; i++) {
        dest[i] = (char *) malloc(strlen(source[i]) + 1);
        strcpy(dest[i], source[i]);
    }
}

void printArray(char * c[], int len) {
    for (int i = 0; i < len; i++) {
        printf("%s\n", c[i]);
    }
}

void slicearray(char ** dest, char * source[], int from, int to) {
    // https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
    int j = 0;
    for (; from <= to; from++, j++) {
        // strdup actually makes a copy of the string and 
        // allocates memory from the heap.
        dest[j] = strdup(source[from]);
    }
}


