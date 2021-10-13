//
//  token.c
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#include "token.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

token
make_token(char* type, char* val)
{
    token tt;
    tt.type = strndup(type, strlen(type));
    tt.val = strndup(val, strlen(val));
    
    assert(tt.type != NULL);
    assert(tt.val != NULL);
    
    return tt;
}

void
free_token(token tt)
{
    free(tt.type);
    free(tt.val);
}
