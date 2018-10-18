/**********************************************************************
* Assignment: Problem Set 2 - Shell recreation                        *
* Author: Gabriel Duarte, Joe Missamore, Mei Williams                 *
* Date: Fall 2018                                                     *
* Description: This will parse all input (within reason) and show     *
*              correct output as if it were part of the bash terminal.*
**********************************************************************/
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

/******************************************************************************
 *  Function:  is_arg_delim                                                   *
 *  Parameters: w (an element in our line_words array)                        *
 *  Returns:   True if it is a matching delimiter, false otherwise            *
 *  Description: This will compare a list element for command purposes        *
 *****************************************************************************/
bool is_arg_delim(char * w) {
    if (strcmp(w, "|") == 0 ||
            strcmp(w, "<") == 0 ||
            strcmp(w, ">") == 0 ||
            strcmp(w, ">>") == 0) {
                return true;
            }
            return false;
}

/******************************************************************************
 *  Function:  parse                                                          *
 *  Parameters: line_words (list of words split), num_words (number of words) *
 *  Returns:   nothing                                                        *
 *  Description: Will parse each line of text entered by user                 *
 *****************************************************************************/

void parse(char ** line_words, int num_words)
{
    // char ** line_words: The entire string of commands

    // number of commands
    // ls -la | grep -a file.txt > output.txt
    // num_commands = 3
    int num_commands = 0;
    
    // length of a given command
    // ls -la = 2
    // grep -a file.txt = 3
    int command_len = 0;

    //TODO: This is statically assigned 
    // Make it dynamic
    struct command commands[100];

    // If a command was failed to be entered...
    if (num_words == 0) { printf("Please enter a command...\n"); return; }

    int iix = 0;
    // Go through all the 'tokens'
    for (; iix < num_words; iix++) {
        // Capturing the 'token'
        char * w = line_words[iix];

        // If it sees a redirect we will capture the entire command.
        if (is_arg_delim(w)) {
            char ** temp = (char **) malloc(((command_len + 1) * sizeof(char *)));
            slicearray(temp, line_words, iix - command_len, iix - 1);
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
    // iix at this point is at the end of the array (out of bounds)
    // needs to be adjusted back to the end of the array (in bounds)
    iix--; // need to back iix up

    char ** temp = (char **) malloc(((command_len + 1) * sizeof(char *)));
    slicearray(temp, line_words, iix - command_len + 1, iix);

    commands[num_commands].command_string = temp;
    commands[num_commands].command_length = command_len;
    commands[num_commands].redirection = NULL;

    num_commands++;

    // make the num_pipes
    int num_pipes = num_commands - 1;
    // get the num_pipe_ends
    int num_pipe_ends = (num_pipes * 2) + 3;
    // dynamically allocate the pipes
    int pfds[num_pipes][2];
    // Initialize the dynamically allocated pipes
    for (int x = 0; x < num_pipes; x++) {
        pipe(pfds[x]);
    }

    for (int j = 0; j < num_commands; j++) {

        int saved;
        int pfd;

        bool WRITE_TO_FILE = false;
        bool APPEND_TO_FILE = false;
        bool CREAT_OVEWRIT_FILE = false;
        bool READ_IN_FR_FILE = false;
        bool READ_IN_AND_APPEND = false;
        bool READ_IN_AND_WRITE = false;

        if (commands[j].redirection != NULL && strcmp(commands[j].redirection, ">") == 0) { WRITE_TO_FILE = true; CREAT_OVEWRIT_FILE = true; }
        else if (commands[j].redirection != NULL && strcmp(commands[j].redirection, ">>") == 0) { WRITE_TO_FILE = true; APPEND_TO_FILE = true; }
        else if (commands[j].redirection != NULL && strcmp(commands[j].redirection, "<") == 0) { READ_IN_FR_FILE = true; }

        if (commands[j+1].redirection != NULL && strcmp(commands[j+1].redirection, ">") == 0) { READ_IN_AND_APPEND = true; }
        else if (commands[j+1].redirection != NULL && strcmp(commands[j+1].redirection, ">>") == 0) { READ_IN_AND_WRITE = true; }

        // If we are writing to file
        if (WRITE_TO_FILE) {
            // flush stdout
            fflush(stdout);
            // save stdout
            saved = dup(1);
            // If user didnt enter file name
            if (commands[j+1].command_string[0] == NULL) {
                printf("Expecting file for redirect > <file>\n");
                break;
            }
            // Someone entered a pipe in the middle of
            // their command sequence like...
            // command1 | command 2 > file | command3
            if (j != num_commands - 2) {
                printf("File redirect in the wrong position...\n");
                break;
            }
            // redirection: >
            if (CREAT_OVEWRIT_FILE) {
                pfd = open(commands[j+1].command_string[0], O_WRONLY | O_CREAT, 0777);
            }
            // redirection: >>
            else if (APPEND_TO_FILE) {
                pfd = open(commands[j+1].command_string[0], O_WRONLY | O_APPEND, 0777);
            }
            dup2(pfd, 1);
            close(pfd);

        }
        else if (READ_IN_FR_FILE) {
            fflush(stdin);
            saved = dup(0);
            if (commands[j+1].command_string[0] == NULL) {
                printf("Expecting file for redirect < <file>\n");
                break;
            }
            else if (commands[j+1].command_length > 1) {
                printf("ERR <filename>\n");
                break;
            }
            pfd = open(commands[j+1].command_string[0], O_RDONLY);
            dup2(pfd, 0);
            close(pfd);
        }
        if (fork() == 0) {
            if (!READ_IN_FR_FILE) {
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
            }
            else if (READ_IN_FR_FILE) {
                if (commands[j+1].redirection != NULL && strcmp(commands[j+1].redirection, "|") == 0) {
                    printf("REDIRECTION < PIPE |\n");
                    dup2(pfds[1][1], 1); // write
                }
                // wc -l < main.c > outputfile
                else if (READ_IN_AND_APPEND) {
                    if (commands[j+2].command_string != NULL ) {
                        fflush(stdout);
                        pfd = open(commands[j+2].command_string[0], O_WRONLY | O_APPEND | O_CREAT, 0777);
                        dup2(pfd, 1);
                        fflush(stdout);
                        close(pfd);
                    }
                }
                // wc -l < main.c >> outputfile
                else if (READ_IN_AND_WRITE) {
                    if (commands[j+2].command_string != NULL ) {
                        fflush(stdout);
                        pfd = open(commands[j+2].command_string[0], O_WRONLY | O_CREAT, 0777);
                        dup2(pfd, 1);
                        fflush(stdout);
                        close(pfd);
                    }
                }
            }
            // Close all open pipes of the child process
            for (int y = 3; y < num_pipe_ends; y++) {
                close(y);
            }
            int err_check = execvp(commands[j].command_string[0], commands[j].command_string);
            if (err_check == - 1) {
                printf("strut shell: command: %s not found...\n", commands[j].command_string[0]);

            }
        }

        if (WRITE_TO_FILE) {
            fflush(stdout);
            dup2(saved, 1);
            close(saved);
            // This should kick us out of the for loop
            j++; // increment j past ( j < num_commands )
        }
        else if (READ_IN_FR_FILE) {
            fflush(stdin);
            dup2(saved, 0);
            close(saved);
            // Increment past file name
            // TODO: This is going to have problems with commands that follow...
            // grep someword < file.txt | wc
            // not going to pick up on pipe
            if (READ_IN_AND_APPEND || READ_IN_AND_WRITE) {
                j += 2;
            }
            else {
                j++;
            }

        }
    }
    //  Close parent pipe fds
    for (int y = 3; y < num_pipe_ends; y++) {
        close(y);
    }
    
}

/******************************************************************************
 *  Function:  slicearray                                                      
 *  Parameters: (char ** dest, char * source[], int from, int to)
 *  Returns:   nothing                                                        
 *  Description: Will copy a *new* slice of a char ** to the dest arg
 *****************************************************************************/
void slicearray(char ** dest, char * source[], int from, int to) {
    int j = 0;
    for (; from <= to; from++, j++) {
        // strdup actually makes a copy of the string and
        // allocates memory from the heap.
        dest[j] = strdup(source[from]);
    }
    dest[j] = NULL;
}