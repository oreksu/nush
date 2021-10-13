//
//  lexer.c
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "utils.h"
#include "token.h"
#include "tvec.h"

static
int
isoper(char chr)
{
    return strchr(OPRCHARS, chr) != NULL;
}

static
int
istext(char chr)
{
    return isalnum(chr) || (ispunct(chr) && !isoper(chr));
}

static
int
isquote(char chr)
{
    return chr == '\"';
}

static
int
isskip(char chr)
{
    return chr == '\\';
}

static
int
isassign(char* line, int ii)
{
    int jj = ii;
    while (1) {
        char curr = line[jj];

        if (curr == '=') {
            return 1;
        }

        if (!istext(curr)) {
            return 0;
        }

        ++jj;
    }
}

// reads one word from the line
static
void
readword(char* line, int* ii, char* val)
{
    int start = *ii;
    while (istext(line[*ii])) {
        *ii += 1;
    }
    
    memcpy(val, line + start, *ii - start);
}

// reads quotes in the line
static
void
readquote(char* line, int* ii, char* val)
{
    *ii += 1; // skip open quote
    
    int start = *ii;
    
    do {
        if (line[*ii] == '\0' || line[*ii] == '\n') {
            perror("--ERROR: No end of the quote\n");
            exit(0);
        }
        
        if (isquote(line[*ii])) {
            memcpy(val, line + start, *ii - start);
            *ii += 1; // skip end quote
            return;
        }
        
        *ii += 1;
        
     } while (1);
}


// reads text from the line
static
void
readtext(char* line, int* ii, char* val)
{
    if (isquote(line[*ii])) {
        readquote(line, ii, val);
    }
    else {
        readword(line, ii, val);
    }
    
    int next = *ii + 2;
    int isnextchar = (istext(line[next]) || isquote(line[next]));
    
    // skip spaces if '\' is found till next char
    if (isskip(line[*ii]) && isnextchar) {
        char* tmp = calloc(1, CMDLINE_MAX);
        readtext(line, &next, tmp);
        strcat(val, tmp);
        *ii = next;
        free(tmp);
    }
}

// transforms given line into vector of tokens
void
lex(char* line, tvec* toks, tvec* venv)
{
    int ii = 0;
    int expect_cmd = 1;
    while (1) {
        char curr = line[ii];
        char next = line[ii + 1];
        
        if (curr == '\0' || curr == '\n') {
            break;
        }

        if (isspace(curr)) {
            ii += 1;
            continue;
        }

        if (curr == '(') {
            tvec_create(toks, "GROUP", "GROUP");
            ii += 1;
            expect_cmd = 1;
            continue;
        }

        if (curr == ')') {
            tvec_create(toks, "GROUP", "END");
            ii += 1;
            continue;
        }

        if (curr == '<') {
            tvec_create(toks, "REDIRECT", "IN");
            ii += 1;
            expect_cmd = 0;
            continue;
        }

        if (curr == '>') {
            tvec_create(toks, "REDIRECT", "OUT");
            ii += 1;
            expect_cmd = 0;
            continue;
        }
        
        if (curr == '=') {
            tvec_create(toks, "ASSIGN", "ASSIGN");
            ii += 1;
            expect_cmd = 0;
            continue;
        }

        if (curr == '|' && next != '|') {
            tvec_create(toks, "DUALOPR", "PIPE");
            ii += 1;
            expect_cmd = 1;
            continue;
        }

        if (curr == '|' && next == '|') {
            tvec_create(toks, "DUALOPR", "OR");
            ii += 2;
            expect_cmd = 1;
            continue;
        }
        
        if (curr == '&' && next == '&') {
            tvec_create(toks, "DUALOPR", "AND");
            ii += 2;
            expect_cmd = 1;
            continue;
        }

        if (curr == '&' && next != '&') {
            tvec_create(toks, "SEPERATOR", "BACK");
            ii += 1;
            expect_cmd = 1;
            continue;
        }

        if (curr == ';') {
            tvec_create(toks, "SEPERATOR", "DELIM");
            ii += 1;
            expect_cmd = 1;
            continue;
        }

        if (isskip(curr)) {
            do { ++ii; } while(isspace(line[ii]));
            continue;
        }
        
        if (curr == '$') {
            int assign = isassign(line, ii);
            
            char* type = (expect_cmd && !assign) ? "CMD" : "ARG";
            char* val = calloc(1, CMDLINE_MAX);
            
            ii += 1;
            readtext(line, &ii, val);
            
            if (tvec_has(venv, val)) {
                strcpy(val, tvec_get(venv, val));
            }
            
            tvec_create(toks, type, val);
            
            free(val);
            expect_cmd = 0;
            continue;
        }

        if (istext(curr) || isquote(curr)) {
            int assign = isassign(line, ii);
            
            char* type = (expect_cmd && !assign) ? "CMD" : "ARG";
            char* val = calloc(1, CMDLINE_MAX);
            readtext(line, &ii, val);
            
            tvec_create(toks, type, val);
            
            free(val);
            expect_cmd = 0;
            continue;
        }
        
        // else give an error
        perror("--ERROR: Unsuported char in input\n");
        exit(1);
    }
}
