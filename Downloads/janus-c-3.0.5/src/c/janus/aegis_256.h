#ifndef AEGIS_256
#define AEGIS_256

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "aes.h"

static const uint8_t const0[16] = {0x00, 0x01, 0x01, 0x02, 0x03, 0x05, 0x08, 0x0d, 0x15, 0x22, 0x37, 0x59, 0x90, 0xe9, 0x79, 0x62};
static const uint8_t const1[16] = {0xdb, 0x3d, 0x18, 0x55, 0x6d, 0xc2, 0x2f, 0xf1, 0x20, 0x11, 0x31, 0x42, 0x73, 0xb5, 0x28, 0xdd};

typedef struct s1 {
    uint8_t s0[AES_BLOCKLEN];
    uint8_t s1[AES_BLOCKLEN];
    uint8_t s2[AES_BLOCKLEN];
    uint8_t s3[AES_BLOCKLEN];
    uint8_t s4[AES_BLOCKLEN];
    uint8_t s5[AES_BLOCKLEN];
} AEGIS_state;

static void AEGIS_initialize(AEGIS_state* S, uint8_t* key, uint8_t* IV);
static void AEGIS_state_update(AEGIS_state* S, uint8_t* m);
static void AEGIS_process_assoc_data(AEGIS_state *S, uint8_t *AD, uint64_t ADlen);
static void AEGIS_encrypt(AEGIS_state* S, uint8_t* C, uint8_t* P, uint64_t MSGlen);
static void AEGIS_decrypt(AEGIS_state* S, uint8_t* C, uint8_t* P, uint64_t MSGlen);
static void AEGIS_finalize(AEGIS_state* S, uint8_t* Tprim, uint64_t adlen, uint64_t msglen);
static void tobinary(uint64_t a, uint8_t* v, uint64_t n);
void AEGIS_tobytes(uint8_t* v, uint64_t n, uint8_t* b);
void AEGIS256_generation_encryption(uint8_t* key, uint8_t* IV, uint8_t* AD, uint64_t ADlen, uint8_t* P, uint8_t* C, uint64_t MSGlen, uint8_t* Tprim);
void AEGIS256_decryption_verification(uint8_t* key, uint8_t* IV, uint8_t* AD, uint64_t ADlen, uint8_t* P, uint8_t* C, uint64_t MSGlen, uint8_t* Tprim);
#endif