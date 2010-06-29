#ifndef __BASE64_H__
#define __BASE64_H__

#include <stdlib.h>

const char *Base64Encode(void *in, size_t in_len);
void *Base64Decode(const char *in, size_t *out_len);

#endif // __BASE64_H__
