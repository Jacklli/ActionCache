/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#ifndef __LOG_H__
#define __LOG_H__

void writeLog(int level, const char *fmt, ...);
void logFile(char *msg, int size);

#endif
