#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "constants.h"
#include "parsetools.h"


int main() {

    // Buffer for reading one line of input
    char line[MAX_LINE_CHARS];
    // Holds separated words based on whitespace
    char* line_words[MAX_LINE_WORDS + 1];
    // Holds a blank space to remove ' and "
    char replace[1] = " ";

    // Loop until user hits Ctrl-D (end of input)
    // or some other input error occurs
    while( fgets(line, MAX_LINE_CHARS, stdin) ) {
        for (int i = 0; i < strlen(line); i++)
        {   // Remove the character ' and "
            if (line[i] == '\"' || line[i] == '\''){
                line[i] = *replace;
            }
        }

        int num_words = split_cmd_line(line, line_words);
        parse(line_words, num_words);
    }

    return 0;
}
