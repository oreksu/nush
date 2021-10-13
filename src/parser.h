//
//  parser.h
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#ifndef parser_h
#define parser_h

#include <stdio.h>

#include "tvec.h"
#include "tnode.h"

tnode* parse(tvec* toks, tnode** nodes, int size);

#endif /* parser_h */

