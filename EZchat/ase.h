#ifndef __ASE_H
#define __ASE_H

#include "global.h"
#include <openssl/aes.h>
int encrypt(char *input, char **encrypt);
void decrypt(char *encrypt, char **decrypt, int len);

#endif