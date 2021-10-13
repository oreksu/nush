//
//  utils.h
//  nush
//
//  Created by Oleksandr Litus on 10/18/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <string.h>

// maximum length of a command line
#define CMDLINE_MAX 256

#define OPRCHARS "& | < > ; ( ) \" = \\"

int streq(char* aa, char* bb);

#endif /* utils_h */
