//
//  tvec.h
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#ifndef tvec_h
#define tvec_h

#include <stdio.h>

#include "token.h"

// vector of tokens
typedef struct tvec {
    int size;
    int capacity;
    token* data;
} tvec;

tvec* make_tvec(void);
void  free_tvec(tvec* toks);

void  tvec_add(tvec* toks, token item);
void  tvec_create(tvec* toks, char* type, char* val);
void  tvec_print(tvec* toks);
void  tvec_render(tvec* toks, char* render);
char* tvec_get(tvec* toks, char* type);
void tvec_change(tvec* toks, char* key, char* value);
int tvec_has(tvec* toks, char* key);

#endif /* tvec_h */
