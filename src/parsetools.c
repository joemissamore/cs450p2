#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "constants.h"
#include "parsetools.h"

/* For open() */
#include <fcntl.h>
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>
/* end open() */

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
            // char ** temp = (char **) malloc(sizeof(command_len + 1));
            char ** temp = (char **) malloc(((command_len + 1) * sizeof(char *))); 
            slicearray(temp, line_words, i - command_len, i - 1);
            // printf("\n\ntemp: %s\n\n", temp[command_len]);
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

    char ** temp = (char **) malloc(((command_len + 1) * sizeof(char *))); 
    
    slicearray(temp, line_words, i - command_len + 1, i);
   
    
    commands[num_commands].command_string = temp;
    commands[num_commands].command_length = command_len;
    commands[num_commands].redirection = NULL;
    
    num_commands++;
   




    if (num_commands == 0) {
        printf("Please enter a command\n");
    } 
    else {

        int num_pipes = num_commands - 1;
        int num_pipe_ends = (num_pipes * 2) + 3;
        int pfds[num_pipes][2];

        for (int x = 0; x < num_pipes; x++) {
            pipe(pfds[x]);
        }

        for (int j = 0; j < num_commands; j++) {

            int saved;
            int pfd;

            
            bool WRITE_TO_FILE = false;
            if (commands[j].redirection != NULL && strcmp(commands[j].redirection, ">") == 0) {
                printf("REDIRECTION >\n");
                WRITE_TO_FILE = true;
                fflush(stdout);
                saved = dup(1);
                pfd = open(commands[j+1].command_string[0], O_WRONLY | O_CREAT, 0777);
                dup2(pfd, 1);
                close(pfd);

            }
            if (fork() == 0) {
                if(!WRITE_TO_FILE && j == 0) {
                    dup2(pfds[0][1], 1); // write
                }
                else if (!WRITE_TO_FILE && j < num_commands - 1) {
                    dup2(pfds[j - 1][0], 0); // read
                    dup2(pfds[j][1], 1); // write
                }
                else if (j == num_commands - 1) {
                    dup2(pfds[j - 1][0], 0); // read
                } 
                // if you are on the last command...
                // still need to read from prev
                // pipes
                // ls -la | grep file > writefile
                else if (WRITE_TO_FILE && j == num_commands - 2) {
                    dup2(pfds[j - 1][0], 0); // read
                }
                for (int y = 3; y < num_pipe_ends; y++) {
                    close(y);
                }
                execvp(commands[j].command_string[0], commands[j].command_string);
            }

            if (WRITE_TO_FILE) {
                fflush(stdout);
                dup2(saved, 1);
                close(saved);
                // This should kick us out of the for loop
                j++; // increment j past ( j < num_commands )
            }
        }
        //  Close parent pipes
        for (int y = 3; y < num_pipe_ends; y++) {
            close(y);
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
    dest[j] = NULL;
}


