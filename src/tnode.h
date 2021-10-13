//
//  tnode.h
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#ifndef tnode_h
#define tnode_h

#include <stdio.h>

#include "token.h"
#include "tvec.h"

typedef struct tnode {
    char* type;
    char* oprcmd;
    char* file;
    struct tnode* left;
    struct tnode* right;
    
    tvec* argv;
    
    char* key;
    char* value;
} tnode;

tnode* make_tnode_group(char* type, char* opr);
tnode* make_tnode_redirect(char* type, char* opr, char* file);
tnode* make_tnode_assign(char* type, char* opr, char* key, char* value);
tnode* make_tnode_dualopr(char* type, char* opr);
tnode* make_tnode_seperator(char* type, char* opr);
tnode* make_tnode_cmd(char* type, char* opr, tvec* argv);
void free_tnode(tnode* tnd);

void tnode_print(tnode* tnd);

#endif /* tnode_h */
