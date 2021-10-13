//
//  eval.h
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#ifndef eval_h
#define eval_h

#include <stdio.h>

#include "tnode.h"

void eval(tnode* tree, tvec* venv);

#endif /* eval_h */
