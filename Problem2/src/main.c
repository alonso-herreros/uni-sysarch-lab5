#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#define USAGE "Usage: palindrome <fileName> [-num]\n"
#define BUFFER_SIZE 1024


typedef struct args {
    int status;
    char *filename;
    int numMode;
} Arguments;



Arguments process_args(int argc, char **argv);
int num_check(char *str);
int str_palindrome(char *str);

Arguments process_args(int argc, char **argv)
{
    Arguments args = {
        .status = 0,
        .filename = NULL,
        .numMode = 0
    };

    if (argc < 2 || argc >3) { // Arg count check
        args.status = 1;
        args.filename = "Wrong number of arguments (2-3 expected)\n";
        return args;
    }

    int numIndex = 2;

    if (argc == 3) { // One arg must be "-num", the other one a filename.
        if (argv[1][0] == '-') { // One arg must start with '-' to be "-num"
            numIndex = 1;
        }
        else if (argv[2][0] == '-') {
            numIndex = 2;
        }
        else {
            args.status = 1;
            args.filename = "Wrong syntax\n";
            return args;
        }

        if (strcmp(argv[numIndex], "-num") != 0) { // '-' argument must be "-num"
            args.status = 1;
            args.filename = "Wrong option (only \"-num\" expected)\n";
            return args;
        }
        args.numMode = 1;
    }
    
    args.filename = argv[3-numIndex];

    return args;
}


int num_check(char *str)
{
    for (int i=0; str[i]!='\0'; i++) {
        if (str[i]=='\n') continue;
        if (str[i]<'0' || str[i]>'9')  return 0;
    }
    return 1;
}


int str_palindrome(char *str) 
{
    int i, j;
    for (j=strlen(str)-1; j>=0; j--) {
        if (str[j] != '\n' && str[j] != ' ')  break;
    }
    for (i=0; i<j; ) {
        if (str[i] != str[j])  return 0;
        i++; j--;
    }
    return 1;
}


int main(int argc, char **argv)
{
    Arguments args = process_args(argc, argv);
    if (args.status) { // Args error
        if (args.filename != NULL) fprintf(stderr, args.filename);
        fprintf(stderr, USAGE);
        exit(EXIT_FAILURE);
    }

    if (access(args.filename, R_OK) == -1 || access(args.filename, W_OK) == -1) {
        fprintf(stderr, "Can't access file '%s', check existence and permissions.\n", args.filename);
        exit(EXIT_FAILURE);
    }
    FILE *fp = fopen(args.filename, "a");

    size_t nchars = BUFFER_SIZE;
    char *line = calloc(nchars, sizeof(char));
    while (!feof(stdin)) {
        if (getline(&line, &nchars, stdin) == -1 || line[0] == '\n')  continue;

        if (args.numMode && !num_check(line)) {
            printf("That was not a number. Try entering a number, or running without the \"-num\" option.\n");
            continue;
        }
        if (str_palindrome(line)) {
            printf("^ That was a palindrome! Adding to file '%s'... ", args.filename);
            int nwritten = fwrite(line, sizeof(char), strlen(line), fp);
            if (nwritten == -1)  printf("Error writing.\n");
            else  printf("Done. Use Ctrl+D to save and exit.\n");
        }
        else {
            printf("^ Not palindrome\n");
        }
    }

    printf("Exiting... ");

    free(line);
    fclose(fp);

    printf("Done.\n");

    exit(EXIT_SUCCESS);
}
