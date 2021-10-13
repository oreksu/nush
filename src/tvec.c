//
//  tvec.c
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#include "tvec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "utils.h"
#include "token.h"

tvec*
make_tvec()
{
    tvec* toks = calloc(1, sizeof(tvec));
    
    toks->size = 0;
    toks->capacity = 2;
    toks->data = calloc(toks->capacity, sizeof(token));
    
    return toks;
}

void
free_tvec(tvec* toks)
{
    if (toks != NULL) {
        if (toks->size != 0) {
            for (int ii = 0; ii < toks->size; ++ii) {
                free_token(toks->data[ii]);
            }
        }
        free(toks->data);
        free(toks);
    }
}

static
void
tvec_put(tvec* toks, int ii, token item)
{
    assert(ii >= 0 && ii < toks->size);
    toks->data[ii] = item;
    //toks->data[ii].type = strdup(item.type);
    //toks->data[ii].val = strdup(item.val);
}

void
tvec_add(tvec* toks, token item)
{
    int ii = toks->size;

    if (ii >= toks->capacity) {
       toks->capacity *= 2;
       toks->data = realloc(toks->data, toks->capacity * sizeof(token));
    }
    
    toks->size += 1;
    tvec_put(toks, ii, item);
}

// creates a token with given params
void
tvec_create(tvec* toks, char* type, char* val)
{
    token tok = make_token(type, val);
    tvec_add(toks, tok);
}

// creates a token with given params
char*
tvec_get(tvec* toks, char* type)
{
    int ii = 0;
    while(1) {
        if (toks->data[ii].type == NULL) {
            return NULL;
        }
        
        if (streq(toks->data[ii].type, type)) {
            return toks->data[ii].val;
        }
        
        ii += 1;
    }
}

// creates token with given value
void
tvec_change(tvec* toks, char* key, char* value)
{
    int ii = 0;
    while(1) {
        if (streq(toks->data[ii].type, key)) {
            toks->data[ii].val = realloc(toks->data[ii].val, strlen(value) + 1);
            strcpy(toks->data[ii].val, value);
            return;
        }
        ii += 1;
    }
}

// is token in this token
int
tvec_has(tvec* toks, char* key)
{
    return tvec_get(toks, key) != NULL;
}

// prints all the tokens in vector for DEBUG
void
tvec_print(tvec* toks)
{
    printf("--DEBUG: TOKENS: ");
    for (int ii = 0; ii < toks->size; ++ii) {
        token tok = toks->data[ii];
        printf("[%s %s] ", tok.type, tok.val);
    }
    printf("\n");
}

// return string that represents values of all tokens
void
tvec_render(tvec* toks, char* render)
{
    strcpy(render, toks->data[0].val);
    
    for (int ii = 1; ii < toks->size; ++ii) {
        strcat(render, " ");
        strcat(render, toks->data[ii].val);
    }
}
