/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#ifndef __CODEC_H__
#define __CODEC_H__

#include "buffer.h"

int decod(buffer *buf);
int encod(buffer *buf, char **argv);


#endif
