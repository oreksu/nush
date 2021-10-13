//
//  parser.c
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "utils.h"
#include "tvec.h"
#include "tnode.h"
#include "token.h"

// gets a subset of toks, that is only consecutive args
static
void
getargv(tvec* toks, int* ii, tvec* argv) {
    char type[32];
    char val[32];
    
    while(*ii < toks->size) {
        int isarg = streq(toks->data[*ii].type, "ARG");
        int isvararg = streq(toks->data[*ii].type, "VARARG");
            
        if (!isarg && !isvararg) {
            break;
        }
        
        strcpy(type, toks->data[*ii].type);
        strcpy(val, toks->data[*ii].val);
        
        tvec_create(argv, type, val);
        
        *ii += 1;
    }
}

// groups tokens into small trees
static
void
group(tnode** nodes, tvec* toks, int size)
{
    int ii = 0;
    int gg = 0;
    while (gg < size) {
        char type[strlen(toks->data[ii].type) + 1];
        strcpy(type, toks->data[ii].type);
        
        char oprcmd[strlen(toks->data[ii].val) + 1];
        strcpy(oprcmd, toks->data[ii].val);
        
        if (streq(type, "GROUP")) {
            nodes[gg] = make_tnode_group(type, oprcmd);

            ii += 1;
            gg += 1;
            continue;
        }

        if (streq(type, "REDIRECT")) {
            int isnextarg = (ii + 1 < toks->size) && streq(toks->data[ii + 1].type, "ARG");
            int isnextvararg = (ii + 1 < toks->size) && streq(toks->data[ii + 1].type, "VARARG");
            if (!isnextarg && !isnextvararg) {
                perror("--ERROR: No arg for redirect\n");
                exit(1);
            }

            char* file = strdup(toks->data[ii + 1].val);
            nodes[gg] = make_tnode_redirect(type, oprcmd, file);

            ii += 2;
            gg += 1;
            continue;
        }
        
        if (streq(type, "ASSIGN")) {
            int isprevarg = (ii - 1 >= 0) && streq(toks->data[ii - 1].type, "ARG");
            int isprevvararg = (ii - 1 >= 0) && streq(toks->data[ii - 1].type, "VARARG");

            int isnextarg = (ii + 1 < toks->size) && streq(toks->data[ii + 1].type, "ARG");
            int isnextvararg = (ii + 1 < toks->size) && streq(toks->data[ii + 1].type, "VARARG");

            if ((!isprevarg || !isprevvararg) && (!isnextarg && !isnextvararg)) {
                perror("--ERROR: No arg for assign\n");
                exit(1);
            }
            
            char key[strlen(toks->data[ii - 1].val) + 1];
            strcpy(key, toks->data[ii - 1].val);
            
            char value[strlen(toks->data[ii + 1].val) + 1];
            strcpy(value, toks->data[ii + 1].val);

            nodes[gg] = make_tnode_assign(type, oprcmd, key, value);
            
            gg += 1;
            ii += 1;
        }

        if (streq(type, "DUALOPR")) {
            nodes[gg] = make_tnode_dualopr(type, oprcmd);
            
            ii += 1;
            gg += 1;
            continue;
        }

        if (streq(type, "SEPERATOR")) {
            nodes[gg] = make_tnode_seperator(type, oprcmd);

            ii += 1;
            gg += 1;
            continue;
        }

        if (streq(type, "CMD")) {
            ii += 1;
            tvec* argv = make_tvec();
            getargv(toks, &ii, argv);
            nodes[gg] = make_tnode_cmd(type, oprcmd, argv);

            gg += 1;
            continue;
        }

        if (streq(type, "VARCMD")) {
            ii += 1;
            tvec* argv = make_tvec();
            getargv(toks, &ii, argv);
            nodes[gg] = make_tnode_cmd(type, oprcmd, argv);

            gg += 1;
            continue;
        }

        if (streq(type, "ARG") || streq(type, "VARARG")) {
            int isnextassign = (ii + 1 < toks->size) && streq(toks->data[ii + 1].val, "ASSIGN");
            if (isnextassign) {
                ii += 1;
                continue;
            }

            // error for too many args
            perror("--ERROR: Too many args.\n");
            exit(1);
        }
    }
}

static
tnode*
combine(tnode** nodes, int size, int level)
{
    char type[CMDLINE_MAX];
    char oprcmd[CMDLINE_MAX];
    
    int ii = 0;
    while (ii < size) {
        strcpy(type, nodes[ii]->type);
        strcpy(oprcmd, nodes[ii]->oprcmd);
        
        if (streq(type, "GROUP") && streq(oprcmd, "GROUP")) {
            level += 1;
        }
        
        if (streq(type, "GROUP") && streq(oprcmd, "END")) {
            level -= 1;
        }
        
        if (!level && streq(type, "SEPERATOR")) {
            
            if (ii == 0 || nodes[ii - 1] == NULL) {
                perror("No commands are given\n");
                exit(0);
            }
            
            nodes[ii]->left = combine(nodes, ii, level);
            
            if (ii + 1 < size && nodes[ii + 1] != NULL) {
                nodes[ii]->right = combine(nodes + ii + 1, size - ii - 1, level);
            }

            return nodes[ii];
        }
        
        ii += 1;
    }
    
    ii = 0;
    level = 0;
    while (ii < size) {
        strcpy(type, nodes[ii]->type);
        strcpy(oprcmd, nodes[ii]->oprcmd);
        
        if (streq(type, "GROUP") && streq(oprcmd, "GROUP")) {
            level += 1;
        }
        
        if (streq(type, "GROUP") && streq(oprcmd, "END")) {
            level -= 1;
        }
        
        if (!level && streq(type, "DUALOPR")) {
            nodes[ii]->left = combine(nodes, ii, level);
            nodes[ii]->right = combine(nodes + ii + 1, size - ii - 1, level);
            return nodes[ii];
        }

        ii += 1;
    }
    
    ii = 0;
    level = 0;
    while (ii < size) {
        strcpy(type, nodes[ii]->type);
        strcpy(oprcmd, nodes[ii]->oprcmd);
        
        if (streq(type, "GROUP") && streq(oprcmd, "GROUP")) {
            level += 1;
        }
        
        if (streq(type, "GROUP") && streq(oprcmd, "END")) {
            level -= 1;
        }
        
        if (!level && streq(type, "REDIRECT")) {
            nodes[ii]->left = combine(nodes, ii, level);
            return nodes[ii];
        }
        
        ii += 1;
    }
    
    ii = 0;
    level = 0;
    while (ii < size) {
        strcpy(type, nodes[ii]->type);
        strcpy(oprcmd, nodes[ii]->oprcmd);
        
        if (streq(type, "GROUP") && streq(oprcmd, "GROUP")) {
            level += 1;
        }
        
        if (streq(type, "GROUP") && streq(oprcmd, "END")) {
            level -= 1;
        }
        
        if (!level && streq(type, "CMD")) {
            if (ii + 1 < size && nodes[ii + 1] != NULL && streq(nodes[ii + 1]->type, "GROUP")) {
                nodes[ii]->right = combine(nodes + ii + 1, size - ii - 1, level);
            }
            return nodes[ii];
        }
        
        if (!level && streq(type, "VARCMD")) {
            if (ii + 1 < size && nodes[ii + 1] != NULL && streq(nodes[ii + 1]->type, "GROUP")) {
                nodes[ii]->right = combine(nodes + ii + 1, size - ii - 1, level);
            }
            return nodes[ii];
        }
        
        if (!level && streq(type, "ASSIGN")) {
            return nodes[ii];
        }
        
        ii += 1;
    }
    
    level = 0;
    for (int ii = 0; ii < size; ++ii) {
        strcpy(type, nodes[ii]->type);
        strcpy(oprcmd, nodes[ii]->oprcmd);

        if (streq(type, "GROUP") && streq(oprcmd, "GROUP")) {
            
            
            // find end of the group and free it, otherwise error
            for (int jj = size - 1; jj < size; ++jj) {
                strcpy(type, nodes[jj]->type);
                strcpy(oprcmd, nodes[jj]->oprcmd);
                
                if (streq(type, "GROUP") && streq(oprcmd, "GROUP")) {
                    free_tnode(nodes[jj]);
                    jj += 1;
                    level +=1;
                }

                if (streq(type, "GROUP") && streq(oprcmd, "END")) {
                    
                    if (level == 0) {
                        free_tnode(nodes[jj]);
                        size -= 1;
                        break;
                    }
                    
                    jj += 1;
                    level -=1;
                }
            }
            
            nodes[ii]->right = combine(nodes + ii + 1, size - ii - 1, level);
            return nodes[ii];
        }
    }
    
    // ERROR
    perror("--ERROR: Type not found in combine\n");
    exit(1);
}


 
tnode*
parse(tvec* toks, tnode** nodes, int size)
{
    // group tokens into small trees
    group(nodes, toks, size);
    assert(nodes != NULL);
    
    // combine small trees into synatax tree
    // level is used to determine where we are insode the '()'
    int level = 0;
    tnode* tree = combine(nodes, size, level);
    assert(tree != NULL);
    
    return tree;
}
