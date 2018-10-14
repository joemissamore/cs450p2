#ifndef PARSETOOLS_H
#define PARSETOOLS_H
#include <stdbool.h>

// Parse a command line into a list of words,
//    separated by whitespace.
// Returns the number of words
//
struct command {
    int command_length; 
    char ** command_string;
};


void strcpyarray(char * dest[], char * source[], int len);
void printArray(char * c[], int len);
void slicearray(char * dest[], char * source[], int from, int to);

bool is_arg_delim(char * c);
int split_cmd_line(char* line, char** list_to_populate); 
void _execute(char **, int, int in_pipe, int out_pipe);
void parse(char ** line_words, int num_words) ;

#endif
