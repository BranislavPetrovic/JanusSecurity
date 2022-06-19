#include "RC5_Janus.h"

static uint16_t ROTL(uint16_t x, uint16_t y, uint16_t w) {
    return ((x) << (y & (w - 1))) | ((x)>>(w - (y & (w - 1))));
}

static uint16_t ROTR(uint16_t x, uint16_t y, uint16_t w) {
    return ((x)>>(y & (w - 1))) | ((x) << (w - (y & (w - 1))));
}

static void RC5_encrypt(uint16_t* pt, uint16_t* ct, uint16_t* S, uint16_t w, uint16_t r) {
    uint16_t A = pt[0] + S[0], B = pt[1] + S[1];
    int i;
    for (i = 1; i <= r; i++) {
        A = ROTL(A^B, B, w) + S[2 * i];
        B = ROTL(B^A, A, w) + S[2 * i + 1];
    }
    ct[0] = A;
    ct[1] = B;
}

static void RC5_decrypt(uint16_t* ct, uint16_t* pt, uint16_t* S, uint16_t w, uint16_t r) {
    uint16_t B = ct[1], A = ct[0];
    int i;
    for (i = r; i > 0; i--) {
        B = ROTR(B - S[2 * i + 1], A, w)^A;
        A = ROTR(A - S[2 * i], B, w) ^B;
    }
    pt[1] = B - S[1];
    pt[0] = A - S[0];
}

static void RC5_initialize(uint8_t* K, uint16_t* S, uint16_t w, uint16_t r, uint16_t b) {
    uint16_t t = 2 * (r + 1);
    uint16_t c = (uint16_t) ceil(8 * b / w);
    uint16_t u = w / 8, A, B, L[c];
    uint16_t P = 0xb7e1, Q = 0x9e37; /* magic constants for a 16-bit word (see the original paper by Rivest) */
    int i, j, k;
    for (i = b - 1, L[c - 1] = 0; i != -1; i--)
        L[i / u] = (L[i / u] << 8) + K[i];
    for (S[0] = P, i = 1; i < t; i++)
        S[i] = S[i - 1] + Q;
    for (A = B = i = j = k = 0; k < 3 * t; k++, i = (i + 1) % t, j = (j + 1) % c) {
        A = S[i] = ROTL(S[i]+(A + B), 3, w);
        B = L[j] = ROTL(L[j]+(A + B), (A + B), w);
    }
}

uint32_t Janus_RC5_encrypt(uint32_t inblock, uint8_t* key) {
    //
    // The RC5 16/12/16 encryption function for Janus
    // The word size is w = 16 bits (since the block size is 32 bits)
    // The number of rounds is r = 12
    // The size of the key is b = 16 bytes
    //
    // Input
    // inblock - the input 32-bit plaintext block
    // key - the 16-byte key
    //
    // Output
    //
    // outblock - the output 32-bit ciphertext block
    //
    uint32_t outblock;
    uint16_t pt[2], ct[2] = {0, 0};
    uint16_t w = 16, r = 12, b = 16;
    uint16_t t = 2 * r + 1;
    uint16_t S[t]; 
    int i;
    pt[0] = (uint16_t) (inblock >> 16);
    pt[1] = (uint16_t) (inblock & 0x0000ffff);
    RC5_initialize(key, S, w, r, b);
    RC5_encrypt(pt, ct, S, w, r);
    outblock = (ct[0] << 16) + ct[1];
    return outblock;
}

uint32_t Janus_RC5_decrypt(uint32_t inblock, uint8_t* key) {
    //
    // The RC5 16/12/16 decryption function for Janus
    // The word size is w = 16 bits (since the block size is 32 bits)
    // The number of rounds is r = 12
    // The size of the key is b = 16 bytes
    //
    // Input
    // inblock - the input 32-bit ciphertext block
    // key - the 16-byte key
    //
    // Output
    //
    // outblock - the output 32-bit plaintext block
    //
    uint32_t outblock;
    uint16_t pt[2], ct[2] = {0, 0};
    uint16_t w = 16, r = 12, b = 16;
    uint16_t t = 2 * r + 1;
    uint16_t S[t]; 
    int i;
    ct[0] = (uint16_t) (inblock >> 16);
    ct[1] = (uint16_t) (inblock & 0x0000ffff);
    RC5_initialize(key, S, w, r, b);
    RC5_decrypt(ct, pt, S, w, r);
    outblock = (pt[0] << 16) + pt[1];
    return outblock;
}