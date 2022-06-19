#include "aes.h"
#include "ccm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Enable ECB, CTR and CBC mode. Note this can be done before including aes.h or at compile-time.
// E.g. with GCC by using the -D flag: gcc -c aes.c -DCBC=0 -DCTR=1 -DECB=1
#define CBC 1
#define CTR 1
#define ECB 1

int main(void) {
    int i;

#if defined(AES256)
    printf("\nTesting AES256\n\n");
#elif defined(AES192)
    printf("\nTesting AES192\n\n");
#elif defined(AES128)
    printf("\nTesting CCM generation/encryption and decryption/verification with AES\n\n");
#else
    printf("You need to specify a symbol between AES128, AES192 or AES256. Exiting");
    return 0;
#endif

    uint8_t key[] = {0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf}; // The key   
    uint8_t N[] = {0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}; // The nonce
    uint8_t A[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; // The associated data
    uint8_t P[] = {0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e}; // The plaintext
    uint8_t n = 13; // Length of the nonce in bytes
    ulong a = 8; // Length of the associated data in bytes
    ulong p = 23; // Length of the plaintext in bytes
    uint8_t t = 8; // Length of the MAC tag in bytes
    uint8_t err;
    printf("The key\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", key[i]);
    printf("\n\n");
    printf("The nonce N\n\n");
    for (i = 0; i < n; i++)
        printf("%02x ", N[i]);
    printf("\n\n");
    printf("The associated data A\n\n");
    for (i = 0; i < a; i++) {
        printf("%02x ", A[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n\n");
    printf("The plaintext P\n\n");
    for (i = 0; i < p; i++) {
        printf("%02x ", P[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n\n");
    printf("The length of the MAC tag T is t = %d\n\n", t);
    // Calling the CCM generation/encryption function to obtain the ciphertext
    ulong c = p + t;
    uint8_t* C = malloc(c * sizeof (uint8_t));
    err = CCM_gen_encrypt(key, N, n, A, a, P, p, t, C, c);
    if (err == 0) {
        printf("The authenticated and encrypted output C\n\n");
        for (i = 0; i < p + t; i++) {
            printf("%02x ", C[i]);
            if ((i + 1) % AES_BLOCKLEN == 0)
                printf("\n");
        }
        printf("\n\n");
    } else {
        printf("Error in input data, error code = %d", err);
    }
    // Calling the CCM decryption/verification function to obtain the plaintext and authenticate
    p = c - t; // We compute p here since the receiver knows c and t, but not p
    for (i = 0; i < n; i++) {
        N[i] = 0x00;
    }
    
    //N[0] = 0xad;
    err = CCM_decrypt_verify(key, C, c, t, N, n, A, a, P, p);
    if (err > 0)
        printf("Authentication failure!\n\n");
    else {
        printf("Authentication successful!\n\n");
        printf("The plaintext P\n\n");
        for (i = 0; i < p; i++) {
            printf("%02x ", P[i]);
            if ((i + 1) % 16 == 0)
                printf("\n");
        }
        printf("\n\n");
    }
    free(C);
    return 0;
}
