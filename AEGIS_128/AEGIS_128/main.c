#include <stdio.h>
#include <stdlib.h>
#include "aegis_128.h"
#include "aes.h"

int main(int argc, char** argv) {
    int i;
    printf("Testing AEGIS-128 authenticated encryption (Wu, Preneel, 2013)\n\n");
    printf("Test vector 1\n\n");
    uint8_t key1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t IV1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t AD1[] = {0x00}; // If no associated data, reserve at least 1 byte, but set ADlen to 0
    uint8_t P1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Tprim1[AES_BLOCKLEN];
    uint64_t ADlen1 = 0;
    uint64_t MSGlen1 = 16;
    uint8_t* C1 = malloc(MSGlen1 * sizeof (uint8_t));
    printf("The key\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", key1[i]);
    printf("\n\n");
    printf("The initialization vector IV\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", IV1[i]);
    printf("\n\n");
    printf("No associated data\n\n");
    printf("The plaintext\n\n");
    for (i = 0; i < MSGlen1; i++)
        printf("%02x ", P1[i]);
    printf("\n\n");
    // Generation/encryption
    AEGIS_generation_encryption(key1, IV1, AD1, ADlen1, P1, C1, MSGlen1, Tprim1);
    printf("The ciphertext\n\n");
    for (i = 0; i < MSGlen1; i++)
        printf("%02x ", C1[i]);
    printf("\n\n");
    printf("The authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim1[i]);
    printf("\n\n");
    // Decryption/verification
    for (i = 0; i < MSGlen1; i++)
        P1[i] = 0;
    for (i = 0; i < AES_BLOCKLEN; i++)
        Tprim1[i] = 0;
    AEGIS_decryption_verification(key1, IV1, AD1, ADlen1, P1, C1, MSGlen1, Tprim1);
    printf("The reconstructed plaintext\n\n");
    for (i = 0; i < MSGlen1; i++)
        printf("%02x ", P1[i]);
    printf("\n\n");
    printf("The reconstructed authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim1[i]);
    printf("\n\n");
    free(C1);
    printf("Test vector 2\n\n");
    uint8_t key2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t IV2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t AD2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t P2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Tprim2[AES_BLOCKLEN];
    uint64_t ADlen2 = 16;
    uint64_t MSGlen2 = 16;
    uint8_t* C2 = malloc(MSGlen2 * sizeof (uint8_t));
    printf("The key\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", key2[i]);
    printf("\n\n");
    printf("The initialization vector IV\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", IV2[i]);
    printf("\n\n");
    printf("The associated data\n\n");
    for (i = 0; i < ADlen2; i++)
        printf("%02x ", AD2[i]);
    printf("\n\n");
    printf("The plaintext\n\n");
    for (i = 0; i < MSGlen2; i++)
        printf("%02x ", P2[i]);
    printf("\n\n");
    // Generation/encryption
    AEGIS_generation_encryption(key2, IV2, AD2, ADlen2, P2, C2, MSGlen2, Tprim2);
    printf("The ciphertext\n\n");
    for (i = 0; i < MSGlen2; i++)
        printf("%02x ", C2[i]);
    printf("\n\n");
    printf("The authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim2[i]);
    printf("\n\n");
    // Decryption/verification
    for (i = 0; i < MSGlen2; i++)
        P2[i] = 0;
    for (i = 0; i < AES_BLOCKLEN; i++)
        Tprim2[i] = 0;
    AEGIS_decryption_verification(key2, IV2, AD2, ADlen2, P2, C2, MSGlen2, Tprim2);
    printf("The reconstructed plaintext\n\n");
    for (i = 0; i < MSGlen2; i++)
        printf("%02x ", P2[i]);
    printf("\n\n");
    printf("The reconstructed authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim2[i]);
    printf("\n\n");
    free(C2);
    printf("Test vector 3\n\n");
    uint8_t key3[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t IV3[] = {0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t AD3[] = {0x00, 0x01, 0x02, 0x03};
    uint8_t P3[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Tprim3[AES_BLOCKLEN];
    uint64_t ADlen3 = 4;
    uint64_t MSGlen3 = 16;
    uint8_t* C3 = malloc(MSGlen3 * sizeof (uint8_t));
    printf("The key\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", key3[i]);
    printf("\n\n");
    printf("The initialization vector IV\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", IV3[i]);
    printf("\n\n");
    printf("The associated data\n\n");
    for (i = 0; i < ADlen3; i++)
        printf("%02x ", AD3[i]);
    printf("\n\n");
    printf("The plaintext\n\n");
    for (i = 0; i < MSGlen3; i++)
        printf("%02x ", P3[i]);
    printf("\n\n");
    // Generation/encryption
    AEGIS_generation_encryption(key3, IV3, AD3, ADlen3, P3, C3, MSGlen3, Tprim3);
    printf("The ciphertext\n\n");
    for (i = 0; i < MSGlen3; i++)
        printf("%02x ", C3[i]);
    printf("\n\n");
    printf("The authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim3[i]);
    printf("\n\n");
    // Decryption/verification
    for (i = 0; i < MSGlen3; i++)
        P3[i] = 0;
    for (i = 0; i < AES_BLOCKLEN; i++)
        Tprim3[i] = 0;
    AEGIS_decryption_verification(key3, IV3, AD3, ADlen3, P3, C3, MSGlen3, Tprim3);
    printf("The reconstructed plaintext\n\n");
    for (i = 0; i < MSGlen3; i++)
        printf("%02x ", P3[i]);
    printf("\n\n");
    printf("The reconstructed authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim3[i]);
    printf("\n\n");
    free(C3);
    printf("Test vector 4\n\n");
    uint8_t key4[] = {0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t IV4[] = {0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t AD4[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t P4[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    uint8_t Tprim4[AES_BLOCKLEN];
    uint64_t ADlen4 = 8;
    uint64_t MSGlen4 = 32;
    uint8_t* C4 = malloc(MSGlen4 * sizeof (uint8_t));
    printf("The key\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", key4[i]);
    printf("\n\n");
    printf("The initialization vector IV\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", IV4[i]);
    printf("\n\n");
    printf("The associated data\n\n");
    for (i = 0; i < ADlen4; i++)
        printf("%02x ", AD4[i]);
    printf("\n\n");
    printf("The plaintext\n\n");
    for (i = 0; i < MSGlen4; i++)
        printf("%02x ", P4[i]);
    printf("\n\n");
    // Generation/encryption
    AEGIS_generation_encryption(key4, IV4, AD4, ADlen4, P4, C4, MSGlen4, Tprim4);
    printf("The ciphertext\n\n");
    for (i = 0; i < MSGlen4; i++)
        printf("%02x ", C4[i]);
    printf("\n\n");
    printf("The authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim4[i]);
    printf("\n\n");
    // Decryption/verification
    for (i = 0; i < MSGlen4; i++)
        P4[i] = 0;
    for (i = 0; i < AES_BLOCKLEN; i++)
        Tprim4[i] = 0;
    AEGIS_decryption_verification(key4, IV4, AD4, ADlen4, P4, C4, MSGlen4, Tprim4);
    printf("The reconstructed plaintext\n\n");
    for (i = 0; i < MSGlen4; i++)
        printf("%02x ", P4[i]);
    printf("\n\n");
    printf("The reconstructed authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim4[i]);
    printf("\n\n");
    free(C4);
    return 0;
}
