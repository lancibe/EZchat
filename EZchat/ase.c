#include "global.h"
#include "ase.h"

//加密、解密函数使用方法：第一个参数填被操作的字符串地址，第二个参数填操作后的字符串地址的地址，第三个参数recv函数的返回值res

//这个函数将输入字符串加密
int encrypt(char *input, char **encrypt)
{
    AES_KEY aes;
    unsigned char key[AES_BLOCK_SIZE];
    unsigned char iv[AES_BLOCK_SIZE];        
     int len;        
     int i;

    len = 0;
    if ((strlen(input) + 1) % AES_BLOCK_SIZE == 0) {
        len = strlen(input) + 1;
    } 
    else {
        len = ((strlen(input) + 1) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    }

    for (i=0; i<16; ++i) {
        key[i] = 32 + i;
    }

    for (i=0; i<AES_BLOCK_SIZE; ++i) {
        iv[i] = 0;
    }
    if (AES_set_encrypt_key(key, 128, &aes) < 0) {
        fprintf(stderr, "Unable to set encryption key in AES\n");
        exit(0);
    }

    *encrypt = (unsigned char*)calloc(len, sizeof(unsigned char));    
    if (*encrypt == NULL) {
        fprintf(stderr, "Unable to allocate memory for encrypt\n");
        exit(-1);
    }

    AES_cbc_encrypt(input, *encrypt, len, &aes, iv, AES_ENCRYPT);
    return len;
}

//这个函数将字符串解密
void decrypt(char *encrypt, char **decrypt,int len)
{
    unsigned char key[AES_BLOCK_SIZE];        
    unsigned char iv[AES_BLOCK_SIZE];        
    AES_KEY aes;
    int i;

    for (i=0; i<16; ++i) {
        key[i] = 32 + i;
    }

    *decrypt = (unsigned char*)calloc(len, sizeof(unsigned char));
    if (*decrypt == NULL) {
        fprintf(stderr, "Unable to allocate memory for decrypt\n");
        exit(-1);
    }

    for (i=0; i<AES_BLOCK_SIZE; ++i) {
        iv[i] = 0;
    }
    if (AES_set_decrypt_key(key, 128, &aes) < 0) {
        fprintf(stderr, "Unable to set decryption key in AES\n");
        exit(-1);
    }

    AES_cbc_encrypt(encrypt, *decrypt, len, &aes, iv, AES_DECRYPT);
}