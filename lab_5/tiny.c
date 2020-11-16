#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <openssl/evp.h>
#include <openssl/err.h>

#define KEY_SIZE 16
#define IV_SIZE  16

static const unsigned char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char reverse_table[128] = {
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
        64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
        64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};


unsigned char *
read_file(const char *, int *);


int
aes_128_ctr_encrypt(unsigned char *plaintext,
                    int plaintext_len,
                    unsigned char *key,
                    unsigned char *iv,
                    unsigned char *ciphertext);


int
aes_128_ctr_decrypt(unsigned char *ciphertext,
                    int ciphertext_len,
                    unsigned char *key,
                    unsigned char *iv,
                    unsigned char *plaintext);


unsigned char *
base64_encode(const unsigned char *, size_t);


unsigned char *
base64_decode(const unsigned char *, size_t *);


int
main(int argc, char *argv[]) {
    if(argc != 5) {
        puts("Usage: program d|e file key init_vector");
        exit(1);
    }
    if(strlen(argv[3]) != KEY_SIZE || strlen(argv[4]) != IV_SIZE) {
        exit(-1);
    }
    unsigned char *key = (unsigned char *)argv[3];
    unsigned char *iv = (unsigned char *)argv[4];

    int file_len;
    unsigned char *plaintext = read_file(argv[2], &file_len);

    if(argv[1][0] == 'e') {
        unsigned char *ciphertext = calloc(file_len, sizeof(char));
        int ciphertext_len = aes_128_ctr_encrypt(plaintext, file_len, key, iv, ciphertext);
        if(ciphertext_len < file_len) {
            puts("Error: aes_128_ctr_encrypt()");
            free(ciphertext);
            return -1;
        }
        unsigned char *base64 = base64_encode(ciphertext, ciphertext_len);
        printf("%s", base64);
        free(base64);
        free(ciphertext);
    }
    else if(argv[1][0] == 'd') {
        size_t base64_len = 0;
        unsigned char *base64 = base64_decode(plaintext, &base64_len);
        unsigned char *decrypted_text = malloc(base64_len);
        int decrypted_len = aes_128_ctr_decrypt(base64, base64_len, key, iv, decrypted_text);
        if(decrypted_len < base64_len) {
            puts("Error: aes_128_ctr_decrypt()");
            free(base64);
            free(decrypted_text);
            exit(1);
        }
        printf("%s", decrypted_text);
        free(base64);
        free(decrypted_text);
    }
    else {
        puts("Flag error! ");
        exit(-1);
    }
    free(plaintext);
    exit(0);
}


unsigned char *
read_file(const char* path, int *len) {
    FILE *fp = fopen (path, "r");
    if(!fp) {
        perror(path);
        exit(-1);
    }
    fseek(fp, 0L, SEEK_END);
    *len = (int)ftell(fp);
    rewind(fp);
    unsigned char *content = calloc(1,*len + 1 );
    if(!content) {
        fclose(fp);
        fputs("memory alloc fails",stderr);
        exit(1);
    }
    if(1 != fread(content, (*len)++, 1, fp)) {
        fclose(fp);
        free(content);
        exit(1);
    }
    fclose(fp);
    return content;
}


unsigned char *
base64_encode(const unsigned char *plaintext, size_t plaintext_size) {
    size_t out_size = ((plaintext_size + 2) / 3) * 4;
    unsigned char * ret_val = malloc(out_size);
    memset(ret_val, '=', out_size);

    size_t out_pos = 0;
    unsigned int bits_collected = 0;
    unsigned int accumulator = 0;
    const unsigned char *bin_end = plaintext + plaintext_size - 1;

    for (unsigned char *i = (unsigned char *)plaintext; i != bin_end; ++i) {
        accumulator = (accumulator << 8u) | (*i & 0xffu);
        bits_collected += 8;
        while (bits_collected >= 6) {
            bits_collected -= 6;
            ret_val[out_pos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
        }
    }
    if (bits_collected > 0) {
        accumulator <<= 6 - bits_collected;
        ret_val[out_pos++] = b64_table[accumulator & 0x3fu];
    }
    return ret_val;
}


unsigned char *
base64_decode(const unsigned char *ciphertext, size_t *out_size) {
    size_t ciphertext_size = strlen((char *)ciphertext);
    *out_size = ((ciphertext_size - 2) * 3) / 4;
    unsigned char * ret_val = calloc(*out_size - 1, sizeof(char));
    const unsigned char *last = ciphertext + ciphertext_size - 1;
    unsigned int bits_collected = 0;
    unsigned int accumulator = 0;

    size_t current_pos = 0;
    for (unsigned char *i = (unsigned char *)ciphertext; i != last; ++i) {
        const int c = *i;
        if (isspace(c) || c == '=') {
            continue;
        }
        if ((c > 127) || (c < 0) || (reverse_table[c] > 63)) {
            puts("Error: illegal character.");
            return NULL;
        }
        accumulator = (accumulator << 6u) | reverse_table[c];
        bits_collected += 6;
        if (bits_collected >= 8) {
            bits_collected -= 8;
            ret_val[current_pos++] += (unsigned char)(accumulator >> bits_collected) & 0xffu;
        }
    }
    --(*out_size);
    return ret_val;
}


int
aes_128_ctr_encrypt(unsigned char *plaintext,
                    int plaintext_len,
                    unsigned char *key,
                    unsigned char *iv,
                    unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx = NULL;
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        ERR_print_errors_fp(stderr);
        return 0;
    }
    if (1 != EVP_EncryptInit_ex (ctx, EVP_aes_128_ctr(), NULL, key, iv)) {
        ERR_print_errors_fp (stderr);
        return 0;
    }
    int len = 0;
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
        ERR_print_errors_fp (stderr);
        return 0;
    }
    int ciphertext_len = len;
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        ERR_print_errors_fp(stderr);
        return 0;
    }
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}


int
aes_128_ctr_decrypt(unsigned char *ciphertext,
                    int ciphertext_len,
                    unsigned char *key,
                    unsigned char *iv,
                    unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx = NULL;
    if (!(ctx = EVP_CIPHER_CTX_new ())) {
        ERR_print_errors_fp(stderr);
        return 0;
    }
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv)) {
        ERR_print_errors_fp(stderr);
        return 0;
    }
    int len = 0;
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
        ERR_print_errors_fp(stderr);
        return 0;
    }
    int plaintext_len = len;
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        ERR_print_errors_fp(stderr);
        return 0;
    }
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

