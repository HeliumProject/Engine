#pragma once

#include <stdlib.h>

const char *Base64Encode(void *in, size_t in_len);
void *Base64Decode(const char *in, size_t *out_len);

