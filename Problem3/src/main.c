#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>

#define BUFFER_SIZE 1024


// ptrlist
typedef struct ptrlist {
    void **ptr;
    struct ptrlist *next;
} ptrlist_t;

/** @brief Appends an element onto the given pointer list 
 * @param head Head to the pointer list
 * @param ptr Pointer to append
 * @return The head to the updated pointer list
 */
ptrlist_t *ptrlist_append(ptrlist_t *head, void *ptr ) {
    if (head == NULL)  head = calloc(1, sizeof(ptrlist_t));
    if (head->ptr == NULL)  head->ptr = ptr;
    else {
        ptrlist_t *new = malloc(sizeof(ptrlist_t));
        new->ptr = ptr;
        new->next = head->next;
        head->next = new;
    }
    return head;
}

/** @brief Replace the first occurence of the old pointer with a new pointer 
 * @param head Head of the pointer list
 * @param ptr_old Old pointer (to be removed)
 * @param ptr_new New pointer
 * @return 0 if the list was updated, 1 otherwise
 */
int ptrlist_replace(ptrlist_t *head, void *ptr_old, void *ptr_new) {
    for(ptrlist_t *curr = head; curr->ptr != NULL; curr = curr->next) {
        if (curr->ptr == ptr_old) {
            curr->ptr = ptr_new;
            return 0;
        }
        if (curr->next == NULL)  break;
    }
    return 1;
}

int ptrlist_remove(ptrlist_t *head, void *ptr) {
    ptrlist_t *last = NULL;
    for(ptrlist_t *curr = head; curr->ptr != NULL; curr = curr->next) {
        if (curr->ptr == ptr) {
            if (curr == head){
                if (curr->next == NULL)  curr->ptr = NULL;
                else {
                    curr->ptr = curr->next->ptr;
                    curr->next = curr->next->next;
                }
            }
            else  last->next = curr->next;
            return 0;
        }
        if (curr->next == NULL)  break;
        last = curr;
    }
    return 1;
}

/** @brief Executes the given operation on each pointer in the given list
 * @param head Head of the pointer list
 * @param operation Function that will accept the pointers
 */
void ptrlist_op(ptrlist_t *head, void (*operation)(void *)) {
    if (head == NULL)  return;
    for(ptrlist_t *curr = head; curr->ptr != NULL; curr = curr->next) {
        operation(curr->ptr);
        if (curr->next == NULL)  break;
    }
}


void exiterrf(char *format, ...);

void *mallocr(size_t size);
void *callocr(size_t nmemb, size_t msize);
FILE *fopenr(char *fpath, char* mode);
int freeall();
int fcloseall();

int check_file(char *fpath);
int command_loop(FILE *output);
int execute_command(char *line, FILE *output);
void terminate(int sig);


// Global variables.
static ptrlist_t *ptrs = NULL; // Pointers to be freed
static ptrlist_t *openfiles = NULL; // Files to be closed

/** @brief Mallocs the specified space and registers it to `ptrs`
 * @param size 
 * @return The pointer to the allocated memory space
 */
void *mallocr(size_t size)
{
    void *ptr = malloc(size);
    ptrs = ptrlist_append(ptrs, ptr);
    return ptr;
}
/** @brief Callocs the specified space and registers it to `ptrs`
 * @param nmemb 
 * @param msize 
 * @return The pointer to the allocated memory space
 */
void *callocr(size_t nmemb, size_t msize)
{
    void *ptr = calloc(nmemb, msize);
    ptrs = ptrlist_append(ptrs, ptr);
    return ptr;
}

void freer(void *ptr) {
    ptrlist_remove(ptrs, ptr);
    free(ptr);
}

/** @brief Opens a file and registers it to `openfiles`
 * @param fpath File path
 * @param mode Opening mode (see `fopen`)
 * @return The file pointer
 */
FILE *fopenr(char *fpath, char* mode)
{
    FILE *fp = fopen(fpath, mode);
    openfiles = ptrlist_append(openfiles, fp);
    return fp;
}


/** @brief Frees all pointers in the ptrs list and empties it
 * @return 0 (no error)
 */
int freeall()
{
    ptrlist_op(ptrs, free);
    if(ptrs != NULL)  free(ptrs);
    ptrs = NULL;
    return 0;
}

/** @brief Closes the given file pointer, accepting a `void *` parameter and casting it.
 * @param fp File pointer, expected to be castable to `FILE *`
 */
void fclosev(void *fp) {
    fclose((FILE *)fp);
}
/** @brief Closes all file pointers in the openfiles list and empties it
 * @return 0 (no error)
 */
int fcloseall() {
    ptrlist_op(openfiles, fclosev);
    if (openfiles!=NULL)  free(openfiles);
    openfiles = NULL;
    return 0;
}


/** @brief Exits with a formatted error message
 * 
 * @param format string to format
 * @param ... arguments for the formatted string
 */
void exiterrf(char *format, ...)
{
    va_list args;  va_start(args, format);
    vfprintf(stderr, format, args);
    terminate(SIGILL);
}


int main(int argc, char **argv)
{
    // Check arg count
    if (argc != 2)  exiterrf("%s requires exactly 1 argument (%d provided)\n", argv[0], argc - 1);

    // Attach signal handlers
    signal(SIGINT, terminate);
    signal(SIGALRM, terminate);

    // Get filepath from argument, check and open file.
    char *fpath = argv[1];
    if (check_file(fpath))  exiterrf("Invalid file '%s'. Check existence and permissions\n", fpath);

    FILE *fp = fopenr(fpath, "a");

    // Command loop. It needs the file to write the results.
    command_loop(fp);

    terminate(SIGINT);
}


/** @brief Checks for existence and writing permissions on the given filepath.  
 * 
 * Given a file path, checks whether the file exists, and whether the process can
 * write to it
 * 
 * @param fpath Path to the file
 * 
 * @return 
 * * 0 if the file checks the conditions,
 * * 1 if the file doesn't exist, or
 * * 2 if the process lacks writing permission
*/
int check_file(char *fpath)
{
    if (access(fpath, F_OK) == -1)  return 1;
    if (access(fpath, W_OK) == -1)  return 2;
    return 0;
}


int command_loop(FILE *output)
{
    size_t nchars = BUFFER_SIZE;
    char *line = mallocr(sizeof(char)*nchars);

    while (!feof(stdin)) {
        char *old_line = line;
        if(getline(&line, &nchars, stdin) == -1 || line[0] == '\n')  continue;
        if (line != old_line)  ptrlist_replace(ptrs, old_line, line);

        line[strlen(line)-1] = '\0';

        if (execute_command(line, output) > 0)  printf("Not Supported\n");
    }

    return 0;
}


int execute_command(char *line, FILE *output)
{
    const char *operation = strtok(line, " ");

    int (* op)(int);
    if (strcmp(operation, "toupper") == 0)  op = toupper;
    else if (strcmp(operation, "tolower") == 0)  op = tolower;
    else  return 1;

    const char *strn = strtok(NULL, " ");
    char *end;
    const long N = strtol(strn, &end, 10);
    if (N<1 || end[0] != '\0')  return 1;

    char *lineout = callocr(BUFFER_SIZE, sizeof(char));
    char *str;
    int linelen = 0;
    for (int i=0; i < N; i++) {
        if ((str = strtok(NULL, " ")) == 0)  return 1;
        if (linelen > 1)  lineout[linelen++] = ' ';
        strcpy(lineout + linelen*sizeof(char), str);
        linelen += strlen(str);
    }
    for (int i = 0; lineout[i] != '\0'; i++)  lineout[i] = op(lineout[i]);

    printf("%s\n", lineout);
    fprintf(output, "%s\n", lineout);

    freer(lineout);

    return 0;
}


void terminate(int sig)
{
    printf("Terminating...\n");
    
    // Watch out, these printf statements include important function calls
    printf("Closing files... %s.\n",     fcloseall()?  "Error":"Done");
    printf("Freeing pointers... %s.\n",  freeall()?    "Error":"Done");

    printf("Terminated\n");

    if (sig == SIGILL)  exit(EXIT_FAILURE);
    else  exit(EXIT_SUCCESS);
}