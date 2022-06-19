#include <stdio.h>
#include <stdlib.h>
#include "aegis_256.h"
#include "aes.h"

int main(int argc, char** argv) {
    int i;
    printf("Testing AEGIS-256 authenticated encryption (Wu, Preneel, 2013)\n\n");
    printf("Test vector 1\n\n");
    uint8_t key1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t IV1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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
    uint8_t key2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t IV2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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
    uint8_t key3[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t IV3[] = {0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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
    uint8_t key4[] = {0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t IV4[] = {0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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
    printf("Test vector 5\n\n");
    uint8_t key5[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t IV5[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t AD5[] = {0x00};
    uint8_t P5[] = {0x00};
    uint8_t Tprim5[AES_BLOCKLEN];
    uint64_t ADlen5 = 0;
    uint64_t MSGlen5 = 0;
    uint8_t* C5 = malloc(MSGlen5 * sizeof (uint8_t));
    printf("The key\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", key5[i]);
    printf("\n\n");
    printf("The initialization vector IV\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", IV5[i]);
    printf("\n\n");
    printf("The associated data\n\n");
    for (i = 0; i < ADlen5; i++)
        printf("%02x ", AD5[i]);
    printf("\n\n");
    printf("The plaintext\n\n");
    for (i = 0; i < MSGlen5; i++)
        printf("%02x ", P5[i]);
    printf("\n\n");
    // Generation/encryption
    AEGIS_generation_encryption(key5, IV5, AD5, ADlen5, P5, C5, MSGlen5, Tprim5);
    printf("The ciphertext\n\n");
    for (i = 0; i < MSGlen5; i++)
        printf("%02x ", C5[i]);
    printf("\n\n");
    printf("The authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim5[i]);
    printf("\n\n");
    // Decryption/verification
    for (i = 0; i < MSGlen5; i++)
        P5[i] = 0;
    for (i = 0; i < AES_BLOCKLEN; i++)
        Tprim5[i] = 0;
    AEGIS_decryption_verification(key5, IV5, AD5, ADlen5, P5, C5, MSGlen5, Tprim5);
    printf("The reconstructed plaintext\n\n");
    for (i = 0; i < MSGlen5; i++)
        printf("%02x ", P5[i]);
    printf("\n\n");
    printf("The reconstructed authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim5[i]);
    printf("\n\n");
    free(C5);
    printf("Test vector 6\n\n");
    uint8_t key6[] = {0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t IV6[] = {0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t AD6[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t P6[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d};
    uint8_t Tprim6[AES_BLOCKLEN];
    uint64_t ADlen6 = 8;
    uint64_t MSGlen6 = 14;
    uint8_t* C6 = malloc(MSGlen6 * sizeof (uint8_t));
    printf("The key\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", key6[i]);
    printf("\n\n");
    printf("The initialization vector IV\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", IV6[i]);
    printf("\n\n");
    printf("The associated data\n\n");
    for (i = 0; i < ADlen6; i++)
        printf("%02x ", AD6[i]);
    printf("\n\n");
    printf("The plaintext\n\n");
    for (i = 0; i < MSGlen6; i++)
        printf("%02x ", P6[i]);
    printf("\n\n");
    // Generation/encryption
    AEGIS_generation_encryption(key6, IV6, AD6, ADlen6, P6, C6, MSGlen6, Tprim6);
    printf("The ciphertext\n\n");
    for (i = 0; i < MSGlen6; i++)
        printf("%02x ", C6[i]);
    printf("\n\n");
    printf("The authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim6[i]);
    printf("\n\n");
    // Decryption/verification
    for (i = 0; i < MSGlen6; i++)
        P6[i] = 0;
    for (i = 0; i < AES_BLOCKLEN; i++)
        Tprim6[i] = 0;
    AEGIS_decryption_verification(key6, IV6, AD6, ADlen6, P6, C6, MSGlen6, Tprim6);
    printf("The reconstructed plaintext\n\n");
    for (i = 0; i < MSGlen6; i++)
        printf("%02x ", P6[i]);
    printf("\n\n");
    printf("The reconstructed authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim6[i]);
    printf("\n\n");
    free(C6);

    printf("Test vector 7\n\n");
    uint8_t key7[] = {0x70, 0x83, 0x50, 0x59, 0x97, 0xf5, 0x2f, 0xdf, 0x86, 0x54, 0x8d, 0x86, 0xee, 0x87, 0xc1, 0x42,
        0x9e, 0xd9, 0x1f, 0x10, 0x8c, 0xd5, 0x63, 0x84, 0xdc, 0x84, 0x02, 0x69, 0xef, 0x7f, 0xdd, 0x73};
    uint8_t IV7[] = {0x18, 0xcd, 0x77, 0x8e, 0x6f, 0x5b, 0x1d, 0x35, 0xd4, 0xca, 0x97, 0x5f, 0xd7, 0x19, 0xa1, 0x7a,
        0xaf, 0x22, 0xc3, 0xeb, 0xa0, 0x19, 0x28, 0xb6, 0xa7, 0x8b, 0xac, 0x58, 0x10, 0xc9, 0x2c, 0x75};
    uint8_t AD7[] = {0xaf, 0x5b, 0x16, 0xa4, 0x80, 0xe6, 0xa1, 0x40, 0x0b, 0xe1, 0x5c, 0x8e, 0x6b, 0x19, 0x4c, 0x2a,
        0xca, 0x17, 0x5e, 0x3b, 0x5c, 0x3f, 0x3f, 0xbb, 0xec, 0xa8, 0x65, 0xf9, 0x39, 0x0a};
    uint8_t P7[] = {0x5d, 0x66, 0x91, 0x27, 0x1e, 0xb1, 0xb2, 0x26, 0x1d, 0x1b, 0x34, 0xfa, 0x75, 0x60, 0xe2, 0x74,
        0xb8, 0x33, 0x73, 0x34, 0x3c, 0x2e, 0x49, 0xb2, 0xb6, 0xa8, 0x2b, 0xc0, 0xf2, 0x0c, 0xee, 0x85,
        0xcd, 0x60, 0x8d, 0x19, 0x5c, 0x1a, 0x16, 0x67, 0x9d, 0x72, 0x04, 0x41, 0xc9, 0x5f, 0xae, 0x86,
        0x63, 0x1f, 0x3f, 0x2c, 0xd2, 0x7f, 0x38, 0xf7, 0x1c, 0xed, 0xc7, 0x9a, 0xac, 0xa7, 0xfd, 0xdd,
        0xbd, 0x4d, 0xa4, 0xee, 0xb9, 0x76, 0x32, 0x36, 0x6d, 0xb6, 0x5c, 0xa2, 0x1a, 0xcd, 0x85, 0xb4,
        0x1f, 0xd1, 0xa9, 0xde, 0x68, 0x8b, 0xdd, 0xff, 0x43, 0x3a, 0x47, 0x57, 0xeb, 0x08, 0x4e, 0x68,
        0x16, 0xdb, 0xc8, 0xff, 0x93, 0xf5, 0x99, 0x58, 0x04};
    uint8_t Tprim7[AES_BLOCKLEN];
    uint64_t ADlen7 = 30;
    uint64_t MSGlen7 = 105;
    uint8_t* C7 = malloc(MSGlen7 * sizeof (uint8_t));
    printf("The key\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", key7[i]);
    printf("\n\n");
    printf("The initialization vector IV\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", IV7[i]);
    printf("\n\n");
    printf("The associated data\n\n");
    for (i = 0; i < ADlen7; i++)
        printf("%02x ", AD7[i]);
    printf("\n\n");
    printf("The plaintext\n\n");
    for (i = 0; i < MSGlen7; i++)
        printf("%02x ", P7[i]);
    printf("\n\n");
    // Generation/encryption
    AEGIS_generation_encryption(key7, IV7, AD7, ADlen7, P7, C7, MSGlen7, Tprim7);
    printf("The ciphertext\n\n");
    for (i = 0; i < MSGlen7; i++)
        printf("%02x ", C7[i]);
    printf("\n\n");
    printf("The authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim7[i]);
    printf("\n\n");
    // Decryption/verification
    for (i = 0; i < MSGlen7; i++)
        P7[i] = 0;
    for (i = 0; i < AES_BLOCKLEN; i++)
        Tprim7[i] = 0;
    AEGIS_decryption_verification(key7, IV7, AD7, ADlen7, P7, C7, MSGlen7, Tprim7);
    printf("The reconstructed plaintext\n\n");
    for (i = 0; i < MSGlen7; i++)
        printf("%02x ", P7[i]);
    printf("\n\n");
    printf("The reconstructed authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim7[i]);
    printf("\n\n");
    free(C7);

    printf("Test vector 8\n\n");
    uint8_t key8[] = {0xc9, 0x26, 0x8f, 0x60, 0x53, 0x54, 0x27, 0x89, 0x74, 0x71, 0x87, 0xda, 0x61, 0x40, 0xb7, 0x56,
        0x60, 0x24, 0xb6, 0x23, 0xdc, 0x96, 0x91, 0xa5, 0x34, 0xbd, 0x73, 0x07, 0x64, 0xb2, 0x0a, 0x63};
    uint8_t IV8[] = {0xb0, 0xcd, 0x1a, 0xd3, 0x8a, 0x01, 0x13, 0x0f, 0x0b, 0x31, 0x2b, 0xe8, 0x5a, 0x9d, 0xd5, 0x70,
        0x93, 0x7c, 0x0f, 0xa1, 0x05, 0x0e, 0xc7, 0xc3, 0xaa, 0x31, 0xbe, 0xfc, 0x40, 0x0b, 0x82, 0x83};
    uint8_t AD8[] = {0x30, 0x91, 0xf6, 0x0c, 0xfb, 0x9f, 0xa8, 0x94, 0x6e, 0x8a, 0x06, 0xb1, 0xa6, 0x63, 0xe6, 0x0b,
        0x54, 0xa2, 0x4f, 0x2e, 0x5f, 0x8e, 0xb5, 0x28, 0x2d, 0x98, 0x0a, 0x21, 0x87, 0x8c, 0x79, 0x4e,
        0xfa, 0xa2, 0xf6, 0xf6, 0x99, 0x30, 0x0e, 0x3f, 0xac, 0xc6, 0x41, 0x97, 0xc5};
    uint8_t P8[] = {0x60, 0xbf, 0x64, 0x4a, 0xbf, 0x31, 0x50, 0x17, 0x22};
    uint8_t Tprim8[AES_BLOCKLEN];
    uint64_t ADlen8 = 45;
    uint64_t MSGlen8 = 9;
    uint8_t* C8 = malloc(MSGlen8 * sizeof (uint8_t));
    printf("The key\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", key8[i]);
    printf("\n\n");
    printf("The initialization vector IV\n\n");
    for (i = 0; i < AES_KEYLEN; i++)
        printf("%02x ", IV8[i]);
    printf("\n\n");
    printf("The associated data\n\n");
    for (i = 0; i < ADlen8; i++)
        printf("%02x ", AD8[i]);
    printf("\n\n");
    printf("The plaintext\n\n");
    for (i = 0; i < MSGlen8; i++)
        printf("%02x ", P8[i]);
    printf("\n\n");
    // Generation/encryption
    AEGIS_generation_encryption(key8, IV8, AD8, ADlen8, P8, C8, MSGlen8, Tprim8);
    printf("The ciphertext\n\n");
    for (i = 0; i < MSGlen8; i++)
        printf("%02x ", C8[i]);
    printf("\n\n");
    printf("The authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim8[i]);
    printf("\n\n");
    // Decryption/verification
    for (i = 0; i < MSGlen8; i++)
        P8[i] = 0;
    for (i = 0; i < AES_BLOCKLEN; i++)
        Tprim8[i] = 0;
    AEGIS_decryption_verification(key8, IV8, AD8, ADlen8, P8, C8, MSGlen8, Tprim8);
    printf("The reconstructed plaintext\n\n");
    for (i = 0; i < MSGlen8; i++)
        printf("%02x ", P8[i]);
    printf("\n\n");
    printf("The reconstructed authentication tag\n\n");
    for (i = 0; i < AES_BLOCKLEN; i++)
        printf("%02x ", Tprim8[i]);
    printf("\n\n");
    free(C8);
    return 0;
}