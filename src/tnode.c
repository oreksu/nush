//
//  tnode.c
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#include "tnode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "utils.h"
#include "tvec.h"

tnode*
make_tnode_group(char* type, char* opr) {
    tnode* tree = calloc(1, sizeof(tnode));
    tree->type = strdup(type);
    
    tree->oprcmd = strdup(opr);
    tree->file = NULL;
    
    tree->left = NULL;
    tree->right = NULL;
    
    tree->argv = NULL;
    
    tree->key = NULL;
    tree->value = NULL;
    return tree;
}

tnode*
make_tnode_redirect(char* type, char* opr, char* file) {
    tnode* tree = calloc(1, sizeof(tnode));
    tree->type = strdup(type);
    
    tree->oprcmd = strdup(opr);
    tree->file = strdup(file);
    
    tree->left = NULL;
    tree->right = NULL;
    
    tree->argv = NULL;
    
    tree->key = NULL;
    tree->value = NULL;
    return tree;
}

tnode*
make_tnode_assign(char* type, char* opr, char* key, char* value) {
    tnode* tree = calloc(1, sizeof(tnode));
    tree->type = strdup(type);
    
    tree->oprcmd = strdup(opr);
    tree->file = NULL;
    
    tree->left = NULL;
    tree->right = NULL;
    
    tree->argv = NULL;
    
    tree->key = strdup(key);
    tree->value = strdup(value);
    return tree;
}

tnode*
make_tnode_dualopr(char* type, char* opr) {
    tnode* tree = calloc(1, sizeof(tnode));
    tree->type = strdup(type);
    
    tree->oprcmd = strdup(opr);
    tree->file = NULL;
    
    tree->left = NULL;
    tree->right = NULL;
    
    tree->argv = NULL;
    
    tree->key = NULL;
    tree->value = NULL;
    return tree;
}

tnode*
make_tnode_seperator(char* type, char* opr) {
    tnode* tree = calloc(1, sizeof(tnode));
    tree->type = strdup(type);
    
    tree->oprcmd = strdup(opr);
    tree->file = NULL;
    
    tree->left = NULL;
    tree->right = NULL;
    
    tree->argv = NULL;
    
    tree->key = NULL;
    tree->value = NULL;
    return tree;
}

tnode*
make_tnode_cmd(char* type, char* cmd, tvec* argv) {
    tnode* tree = calloc(1, (sizeof(tnode)));
    tree->type = strdup(type);
    
    tree->oprcmd = strdup(cmd);
    tree->file = NULL;
    
    tree->left = NULL;
    tree->right = NULL;
    
    tree->argv = argv;
    
    tree->key = NULL;
    tree->value = NULL;
    return tree;
}

void
free_tnode(tnode* tree)
{
    assert(!streq(tree->type, "ARG") || !streq(tree->type, "VARARG"));

    if (streq(tree->type, "GROUP")) {
        
        free(tree->type);
        
        free(tree->oprcmd);
        free(tree->file);
        
        if (tree->left != NULL) {
            free_tnode(tree->left);
        }
        if (tree->right != NULL) {
            free_tnode(tree->right);
        }
        
        free_tvec(tree->argv);
        
        free(tree->key);
        free(tree->value);
        
        free(tree);
        
        return;
    }
    
    if (streq(tree->type, "REDIRECT")) {
        
        free(tree->type);
        
        free(tree->oprcmd);
        free(tree->file);
        
        if (tree->left != NULL) {
            free_tnode(tree->left);
        }
        if (tree->right != NULL) {
            free_tnode(tree->right);
        }
        
        free_tvec(tree->argv);
        
        free(tree->key);
        free(tree->value);
        
        free(tree);
        
        return;
    }
    
    if (streq(tree->type, "ASSIGN")) {
        
        free(tree->type);
        
        free(tree->oprcmd);
        free(tree->file);
        
        if (tree->left != NULL) {
            free_tnode(tree->left);
        }
        if (tree->right != NULL) {
            free_tnode(tree->right);
        }
        
        free_tvec(tree->argv);
        
        free(tree->key);
        free(tree->value);
        
        free(tree);
        
        return;
    }
    
    if (streq(tree->type, "DUALOPR")) {
        
        free(tree->type);
        
        free(tree->oprcmd);
        free(tree->file);
        
        if (tree->left != NULL) {
            free_tnode(tree->left);
        }
        if (tree->right != NULL) {
            free_tnode(tree->right);
        }
        
        free_tvec(tree->argv);
        
        free(tree->key);
        free(tree->value);
        
        free(tree);
        
        return;
    }
    
    if (streq(tree->type, "SEPERATOR")) {
        
        free(tree->type);
        
        free(tree->oprcmd);
        free(tree->file);
        
        if (tree->left != NULL) {
            free_tnode(tree->left);
        }
        if (tree->right != NULL) {
            free_tnode(tree->right);
        }
        
        free_tvec(tree->argv);
        
        free(tree->key);
        free(tree->value);
        
        free(tree);
        
        return;
    }

    if (streq(tree->type, "CMD") || streq(tree->type, "VARCMD")) {
        
        free(tree->type);
        
        free(tree->oprcmd);
        free(tree->file);
        
        if (tree->left != NULL) {
            free_tnode(tree->left);
        }
        if (tree->right != NULL) {
            free_tnode(tree->right);
        }
        
        free_tvec(tree->argv);
        
        free(tree->key);
        free(tree->value);
        
        free(tree);
        
        return;
    }
    
    // ERROR
    perror("--ERROR: Type not found in free_node\n");
    exit(1);
}

static
void
tnode_render(tnode* tree, char* treestr)
{
    char* type = tree->type;
    assert(!streq(type, "ARG") || !streq(type, "VARARG"));
    
    if (streq(type, "GROUP")) {
        strcat(treestr, "(");
        strcat(treestr, tree->oprcmd);
        strcat(treestr, " ");
        tnode_render(tree->right, treestr);
        strcat(treestr, ")");
        return;
    }
    
    if (streq(type, "ASSIGN")) {
        strcat(treestr, "(");
        strcat(treestr, tree->oprcmd);
        strcat(treestr, " ");
        strcat(treestr, tree->key);
        strcat(treestr, " ");
        strcat(treestr, tree->value);
        strcat(treestr, ")");
        return;
    }
    
    if (streq(type, "REDIRECT")) {
        strcat(treestr, "(");
        strcat(treestr, tree->oprcmd);
        strcat(treestr, " ");
        tnode_render(tree->left, treestr);
        strcat(treestr, " ");
        strcat(treestr, tree->file);
        strcat(treestr, ")");
        return;
    }
    
    if (streq(type, "DUALOPR")) {
        strcat(treestr, "(");
        strcat(treestr, tree->oprcmd);
        strcat(treestr, " ");
        tnode_render(tree->left, treestr);
        strcat(treestr, " ");
        tnode_render(tree->right, treestr);
        strcat(treestr, ")");
        return;
    }
    
    if (streq(type, "SEPERATOR")) {
        strcat(treestr, "(");
        strcat(treestr, tree->oprcmd);
        strcat(treestr, " ");
        tnode_render(tree->left, treestr);
        
        if (tree->right != NULL) {
            strcat(treestr, " ");
            tnode_render(tree->right, treestr);
        }
        
        strcat(treestr, ")");
        return;
    }
     
    if (streq(type, "CMD") || streq(type, "VARCMD")) {
        strcat(treestr, "(");
        strcat(treestr, tree->oprcmd);
        
        if (tree->argv->size != 0) {
            strcat(treestr, " ");
            char* tvecstr = calloc(1, CMDLINE_MAX);
            tvec_render(tree->argv, tvecstr);
            strcat(treestr, tvecstr);
            
            free(tvecstr);
        }
        
        if (tree->right != NULL) {
            strcat(treestr, " ");
            tnode_render(tree->right, treestr);
        }
        
        strcat(treestr, ")");
        return;
    }
}

// prints tree for debugging
void
tnode_print(tnode* tree)
{
    char* type = tree->type;
    assert(!streq(type, "ARG") || !streq(type, "VARARG"));
    
    char* treestr = calloc(1, CMDLINE_MAX);
    strcpy(treestr, "--DEBUG: TREE: ");
    
    tnode_render(tree, treestr);
    
    printf("%s\n", treestr);
    free(treestr);
}
