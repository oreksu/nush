//
//  lexer.h
//  nush
//
//  Created by Oleksandr Litus on 10/17/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#ifndef lexer_h
#define lexer_h

#include <stdio.h>

#include "tvec.h"

void lex(char* line, tvec* toks, tvec* venv);

#endif /* lexer_h */
