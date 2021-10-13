//
//  token.h
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#ifndef token_h
#define token_h

#include <stdio.h>

typedef struct token {
    char* type;
    char* val;
} token;

token make_token(char* type, char* val);
void free_token(token tt);

#endif /* token_h */
