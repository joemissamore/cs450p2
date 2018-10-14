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
            // strcmp(w, "<<") == 0 ||
            strcmp(w, ">") == 0 || 
            strcmp(w, ">>") == 0) {
                return true;
            }
            return false;
}

void parse(char ** line_words, int num_words) 
{

    int num_commands = 0;
    int command_len = 0;
    struct command commands[100];
    
    int i = 0;
    for (; i < num_words; i++) {
        char * w = line_words[i];
            printf("w: %s\n", w);
            if (is_arg_delim(w)) {
                char * temp[command_len];
                slicearray(temp, line_words, i - command_len, i - 1);
                // command_len++;
                commands[num_commands].command_length = command_len;
                commands[num_commands].command_string = temp;
                commands[num_commands].redirection = w;
                
                

                
                // DEBUG
                printf("DEBUG\n");
                // printf("command_len: %d\n", command_len);
                // printf("i: %d\n", i);
                // printf("command: \n");
                // printArray(temp, command_len);
                printArray(commands[num_commands].command_string, commands[num_commands].command_length);
                // printf("redirection: %s\n", commands[num_commands].redirection);

                num_commands++;
                command_len = 0;
        }
        else {
            command_len++;
        }
        
    }
    // i at this point is at the end of the array (out of bounds)
    // needs to be adjusted back to the end of the array (in bounds)
    i--; // need to back i up
    char * temp[command_len];
    slicearray(temp, line_words, i - command_len + 1, i);
    commands[num_commands].command_string = temp;
    commands[num_commands].command_length = command_len;
    commands[num_commands].redirection = NULL;
    
    // DEBUG
    printf("num_commands: %d\n", num_commands);
    printArray(commands[num_commands].command_string, commands[num_commands].command_length);
    // printf("redirection after: %s\n", commands[num_commands].redirection);
    // printf("i: %d\n", i);
    // printf("command_len: %d\n", command_len);
    // printf("command: \n");
    // printArray(temp, command_len);
    // printf("num_commands: %d\n", num_commands);


    // 
    num_commands++;





    if (num_commands == 1) {
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
    else {
        //TODO: handle pipes
        int pfd[4];
        pipe(pfd);
        int j = 0;
        printf("IN ELSE STATEMENT\n");
        for (; j < num_commands - 1; j++) {
            // printf("command_string: %s", commands[j].command_string);
            // printf("command_length: %d", commands[j].command_length);
            printArray(commands[0].command_string, commands[0].command_length);
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
    char * command = line_words[0];
    printf("Executing command: %s\n", command);

    if (in_pipe != -1) {
        dup2(in_pipe, 0);
    }
    if (out_pipe != -1) {
        dup2(out_pipe, 1);
    }

    
    // close(in_pipe);
    // close(out_pipe);
    int err_check = execvp(command, line_words);
    if (err_check == -1) {
        printf("Error with command: %s\n", command);
    }
}











void strcpyarray(char * dest[], char * source[], int len) {
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

void slicearray(char * dest[], char * source[], int from, int to) {
    for (int i = from, j =0; i <= to; i++, j++) {
        dest[j] = (char *) malloc(strlen(source[i]) + 1);
        strcpy(dest[j], source[i]);
    }
}


