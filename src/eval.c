//
//  eval.c
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <fcntl.h>

#include "utils.h"
#include "tnode.h"

static int neval(tnode* tree, tvec* venv, int isback, int isin, char* inf, int isout, char* outf);

static
void
printrelation(int myid, int parentid)
{
    printf("Hi I'm %d, child of %d\n", myid, parentid);
}




static
int
evaland(tnode* node, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;
    
    status = neval(node->left, venv, isback, isin, inf, isout, outf);
    
    if (!status) {
        neval(node->right, venv, isback, isin, inf, isout, outf);
    }
    
    status = 0;
    
    return status;
}


static
int
evalor(tnode* tree, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;

    status = neval(tree->left, venv, isback, isin, inf, isout, outf);
    
    if (status) {
        neval(tree->right, venv, isback, isin, inf, isout, outf);
    }
    
    status = 0;
    
    return status;
}


static
int
evaldelim(tnode* tree, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;
     
    status = neval(tree->left, venv, isback, isin, inf, isout, outf);
    status = neval(tree->right, venv, isback, isin, inf, isout, outf) || status;
     
    return status;
}

static
int
evalback(tnode* node, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;
    
    isback = 1;
    
    if (node->right != NULL) {
        neval(node->left, venv, !isback, isin, inf, isout, outf);
    }

    neval(node->left, venv, isback, isin, inf, isout, outf);
    
    return status;
}


/*
static
int
evalback(tnode* node, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;

    // IN PARENT
    int cpid;
    if ((cpid = fork())) {
        assert(cpid != 0);
        
        if (node->right != NULL) {
            neval(node->left, venv, isback, isin, inf, isout, outf);
        }
        
        // DO NOT WAIT FOR CHILD
        
        return status;
    }
    
    
    // IN CHILD
    else {
        // printf("\nevalcmd: IN CHILD\n");
        
        // execute command in child
        neval(node->left, venv, isback, isin, inf, isout, outf);
        
        exit(0);
    }
}
*/

static
int
evalgroup(tnode* node, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;
    
    // IN PARENT
    int cpid;
    if ((cpid = fork())) {
        assert(cpid != -1);
        
        if (!isback) {
            waitpid(cpid, &status, 0);
        }
    }
    
    // IN CHILD
    else {
        
        // execute command in child
        status = neval(node->right, venv, isback, isin, inf, isout, outf);
        
        exit(0);
    }
    
    return status;
}


void
check_rv(int rv)
{
    if (rv == -1) {
        perror("fail");
        exit(1);
    }
}


static
int
evalpipe(tnode* node, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;
    int rv;
    
    int cpid = fork();
     
    // PARENT
    if (cpid) {
        // printf("\nIN PAR\n");
        
        // wait for child
        waitpid(cpid, &rv, 0);
        
        // printf("\nOUT PAR\n");
        return status;
    }
    
    // CHILD
    if (cpid == 0) {
        // printf("\nIN CHILD\n");
        
        int pipes[2];
        rv = pipe(pipes);
        check_rv(rv);
        
        int p_read = pipes[0];
        int p_write = pipes[1];
        
        int cpidch = fork();
        
        // CHILD
        if (cpidch) {
            close(p_read);
            
            close(1); // close stdout
            rv = dup(p_write);
            check_rv(rv);
            close(p_write);
            
            neval(node->left, venv, isback, isin, inf, isout, outf);
            
            close(1); // WHY?
            
            // wait for grandchild 1
            waitpid(cpidch, &rv, 0);
            
            exit(0);
        }
        
        // GRANDCHILD
        if (cpidch == 0) {
            close(p_write);
            
            close(0); // close stdin
            rv = dup(p_read);
            check_rv(rv);
            close(p_read);
            
            neval(node->right, venv, isback, isin, inf, isout, outf);
            exit(0);
        }
    }
    
    return status;
}


static
int
evalassin(tnode* node, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;

    isin = 1;
    inf = node->file;
    
    status = neval(node->left, venv, isback, isin, inf, isout, outf);

    return status;
}


static
int
evalassout(tnode* node, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;
    
    isout = 1;
    outf = node->file;
    
    // printf("Filename out to: %s\n", outf); // DEBUG
    
    status = neval(node->left, venv, isback, isin, inf, isout, outf);
    
    return status;
}


static
int
evalcmd(tnode* node, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;

    // IN PARENT
    int cpid;
    if ((cpid = fork())) {
        assert(cpid != 0);
        
        // printf("\nevalcmd: IN PARENT\n"); // DEBUG
        
        if (!isback) {
            waitpid(cpid, &status, 0);
        }
        
        //printf("\nevalcmd: OUT OF CHILD\n"); // DEBUG
        //printf("\nevalcmd: OUT OF PARENT\n"); // DEBUG
        
        return status;
    }
    
    
    // IN CHILD
    else {
        // printf("\nevalcmd: IN CHILD\n");
        
        char* cmd = node->oprcmd;
        
        if (isout) {
            freopen(outf, "a+", stdout);
        }
        
        if (isin) {
            freopen(inf, "r", stdin);
        }
        
        int size = (node->argv != NULL) ? (node->argv->size + 2) : 2;
        
        char* argv[size];
        
        argv[0] = cmd;
        
        if (node->argv != NULL) {
            for (int ii = 1; ii < size; ++ii) {
                argv[ii] = node->argv->data[ii - 1].val;
            }
        }
        
        argv[size - 1] = 0;
        
        // execvp overtakes the process with given cmd and args
        execvp(cmd, argv);
        
        // in case execvp fails to overtake the process
        assert(0 && "execvp should overtake the process");
    }
    
    return status;
}


// RECURSIVE EVAL OF ONE NODE
static
int
neval(tnode* node, tvec* venv, int isback, int isin, char* inf, int isout, char* outf)
{
    int status = 0;
    
    // NODE IS A COMMAND
    if (streq(node->type, "CMD")) {
        
        // TRUE - in shell
        if (streq(node->oprcmd, "true")) {
            status = 0;
            return status;
        }
        
        // FALSE - in shell
        if (streq(node->oprcmd, "false")) {
            status = 1;
            return status;
        }
        
        // EXIT - in shell
        if (streq(node->oprcmd, "exit")) {
            free_tvec(venv);
            exit(0);
        }
        
        // CD - in shell
        if (streq(node->oprcmd, "cd")) {
            if (chdir(node->argv->data[0].val) != 0)
                perror("--ERROR: chdir() failed");
            status = 0;
            return status;
        }
        
        // EVAL SINGLE COMMAND
        status = evalcmd(node, venv, isback, isin, inf, isout, outf);
        return status;
    }
    
    // NODE IS A DUAL OPERATOR ['&&' '||' '|']
    if (streq(node->type, "DUALOPR")) {
        
        // AND
        if (streq(node->oprcmd, "AND")) {
            status = evaland(node, venv, isback, isin, inf, isout, outf);
            return status;
        }
            
        // OR
        if (streq(node->oprcmd, "OR")) {
            status = evalor(node, venv, isback, isin, inf, isout, outf);
            return status;
        }
        
        // PIPE
        if (streq(node->oprcmd, "PIPE")) {
            status = evalpipe(node, venv, isback, isin, inf, isout, outf);
            return status;
        }
    }
    
    // NODE IS A REDIRECT ['>' '<']
    if (streq(node->type, "REDIRECT")) {
        
        // printf("Inside the REDIRECT\n"); // DEBUG
        
        if (streq(node->oprcmd, "IN")) {
            status = evalassin(node, venv, isback, isin, inf, isout, outf);
        }
        
        if (streq(node->oprcmd, "OUT")) {
            
            // printf("Inside the OUT\n"); // DEBUG
            
            status = evalassout(node, venv, isback, isin, inf, isout, outf);
        }
        
        return status;
    }
    
    // NODE IS A SEPERATOR ['&' ';']
    if (streq(node->type, "SEPERATOR")) {
        
        // DELIMETER
        if (streq(node->oprcmd, "DELIM")) {
            status = evaldelim(node, venv, isback, isin, inf, isout, outf);
            return status;
        }
        
        // BACKGROUND
        if (streq(node->oprcmd, "BACK")) {
            status = evalback(node, venv, isback, isin, inf, isout, outf);
            return status;
        }
    }
    
    // NODE IS A GROUP
    if (streq(node->type, "GROUP")) {
        evalgroup(node, venv, isback, isin, inf, isout, outf);
    }
    
    // NODE IS A ASSIGNMENT
    if (streq(node->type, "ASSIGN")) {
        char* key = node->key;
        char* value = node->value;
        
        if (tvec_has(venv, key)) {
            tvec_change(venv, key, value);
        }
        else {
            tvec_create(venv, key, value);
        }
        
        status = 0;
        
        return status;
    }
    
    return status;          // 0 - true
}


// MAIN EVAL OF TREE STARTING FROM TOP NODE
void
eval(tnode* tree, tvec* venv)
{
    int isback = 0;         // 1 - true
    int isin = 0;
    char inf[32];
    int isout = 0;
    char outf[32];
    int status = neval(tree, venv, isback, isin, inf, isout, outf);
    
    if (status) {
        free_tvec(venv);
        perror("returned status = 1");
        exit(1);
    }
}
