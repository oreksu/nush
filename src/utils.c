//
//  utils.c
//  nush
//
//  Created by Oleksandr Litus on 10/18/19.
//  Copyright © 2019 Oleksandr Litus. All rights reserved.
//

#include "utils.h"

#include <string.h>

int
streq(char* aa, char* bb)
{
    return strcmp(aa, bb) == 0;
}
