/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "log.h"

int main() {
    writeLog(1, "log here:%s", "abc");
    return 0;
}
