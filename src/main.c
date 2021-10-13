//
//  main.c
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

#include "utils.h"
#include "token.h"
#include "tvec.h"
#include "tnode.h"
#include "lexer.h"
#include "parser.h"
#include "eval.h"

// maximum length of the command line
#define CMDLINE_MAX 256

static
void
show_promt()
{
    printf("\n");
    printf("nush$ ");
}

// returns index 0, if line ends with '\'
static
int
isskip(char* line)
{
    int ll = strlen(line);
    
    for (int ii = ll - 1; ii >= 0; --ii) {
        if ('\\' == line[ii]) {
            return 0;
        }

        if (!isspace(line[ii])) {
            return 1;
        }
    }
    
    return 1;
}

// reads one line from a file, skips "\ \n"
// return fgets status
static
int
readln(char* line, FILE* file)
{
    // gets one line
    const char* rv = fgets(line, CMDLINE_MAX, file);

    // in case line ends with '\', continues to the next line
    if (!isskip(line)) {
        char* tmp = malloc(CMDLINE_MAX);
        if (readln(tmp, file)) {
            char* rv = strcat(line, tmp);
            assert(rv != NULL);
        }
        free(tmp);
    }
    
    // printf("--DEBUG: LINE: %s", line); // DEBUG

    return (rv != NULL);
}

// counts number of small group trees among tokens
static
int
countgroups(tvec* toks)
{
    int count = 0;
    for (int ii = 0; ii < toks->size; ++ii) {
        char* type = toks->data[ii].type;
        
        int issep = streq(type, "SEPERATOR");
        int isdual = streq(type, "DUALOPR");
        int isredir = streq(type, "REDIRECT");
        int isgroup = streq(type, "GROUP");
        int isassign = streq(type, "ASSIGN");
        
        int isopr = issep || isdual || isredir || isgroup || isassign;
        int iscmd = streq(type, "CMD") || streq(type, "VARCMD");
        
        if (isopr || iscmd) {
            ++count;
        }
    }
    
    return count;
}

// process: lex -> parse -> evaluate
static
void
process(char* line, tvec* venv)
{
    // lexing line into tokens, and assigning types
    tvec* toks = make_tvec();
    lex(line, toks, venv);
    assert(toks != NULL);
    
    // no input
    if (toks->size == 0) {
        return;
    }
    
    // tvec_print(toks); // DEBUG
    
    // fininding number of nodes in the tree
    int size = countgroups(toks);
    
    // parsing tokens into syntax tree
    tnode* nodes[64];
    tnode* tree = parse(toks, nodes, size);
    
    // free vector of tokens
    free_tvec(toks);
    
    // tnode_print(tree); // DEBUG
    
    // evaluate commands from the syntax tree
    eval(tree, venv);

    // free ssyntax tree
    free_tnode(tree);
}

// execution of the script from the given file
static
void
execute_script(FILE* file, tvec* venv)
{
    char* line = calloc(1, CMDLINE_MAX);
    while (readln(line, file)) {
        process(line, venv);
    }
    free(line);
}

// interective shell loop, can exit with ^D
static
void
interective_shell(tvec* venv)
{
    while(1) {
        // allocate line on heap
        char* line = calloc(1, CMDLINE_MAX);
        
        show_promt();

        int rv = fflush(stdout);
        assert(rv == 0 && "fflush failed");

        // reading line from terminal, exiting if it's eof
        if (!readln(line, stdin)) {
            free_tvec(venv);
            free(line);
            exit(0);
        }

        // procces the line
        process(line, venv);
        
        // free the line
        free(line);
    }
}

int
main(int argc, const char* argv[])
{
    assert(argc > 0 && argc <= 2 && "too many args");
    
    // GLOBAL VARS
    tvec* venv = make_tvec();
    
    // enter interective loop, if no file is provided
    if (argc == 1) {
        interective_shell(venv);
    }

    // execution of a single file, if provided
    if (argc == 2) {
        const char* filename = argv[1];
        FILE* file = fopen(filename, "r");
        
        assert(file != NULL && "failed to open file");

        execute_script(file, venv);
        
        fclose(file);
        
    }
    
    free_tvec(venv);
    return 0;
}
