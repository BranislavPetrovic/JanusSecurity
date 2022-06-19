#include "aegis_128.h"

static void AEGIS_initialize(AEGIS_state* S, uint8_t* key, uint8_t* IV) {
    //
    // The AEGIS-128 initialization function
    //
    // Input
    //
    // key - the key of length AES_KEYLEN bytes (16 bytes for AEGIS-128)
    // IV - the initialization vector of length AES_KEYLEN bytes (16 bytes for AEGIS-128)
    //
    // Output
    //
    // S - the initial state of AEGIS-128 (the registers s0 .. s4 of length AES_BLOCKLEN)
    //
    int i;
    uint8_t key_plus_IV[AES_BLOCKLEN];
    for (i = 0; i < AES_BLOCKLEN; i++)
        key_plus_IV[i] = key[i]^IV[i];
    for (i = 0; i < AES_BLOCKLEN; i++) {
        S->s0[i] = key_plus_IV[i];
        S->s1[i] = const1[i];
        S->s2[i] = const0[i];
        S->s3[i] = key[i]^const0[i];
        S->s4[i] = key[i]^const1[i];
    }
    for (i = 0; i < 10; i++) {
        if (i % 2 == 0)
            AEGIS_state_update(S, key);
        else
            AEGIS_state_update(S, key_plus_IV);
    }
    return;
}

static void AEGIS_state_update(AEGIS_state* S, uint8_t* m) {
    //
    // The AEGIS-128 state update function
    //
    // Input
    //
    // m - the message (16 bytes)
    //
    // Input/output
    //
    // S - the state of AEGIS-128 (the registers s0 .. s4 of length AES_BLOCKLEN)
    //
    int i;
    AEGIS_state S1;
    uint8_t s0_plus_m[AES_BLOCKLEN];
    for (i = 0; i < AES_BLOCKLEN; i++)
        s0_plus_m[i] = S->s0[i]^m[i];
    // Running AES round function 5 times
    AES_round((state_t*) (S->s4), (state_t*) (S1.s0), s0_plus_m);
    AES_round((state_t*) (S->s0), (state_t*) (S1.s1), S->s1);
    AES_round((state_t*) (S->s1), (state_t*) (S1.s2), S->s2);
    AES_round((state_t*) (S->s2), (state_t*) (S1.s3), S->s3);
    AES_round((state_t*) (S->s3), (state_t*) (S1.s4), S->s4);
    // Updating of the state S
    memcpy(S->s0, S1.s0, AES_BLOCKLEN);
    memcpy(S->s1, S1.s1, AES_BLOCKLEN);
    memcpy(S->s2, S1.s2, AES_BLOCKLEN);
    memcpy(S->s3, S1.s3, AES_BLOCKLEN);
    memcpy(S->s4, S1.s4, AES_BLOCKLEN);
    return;
}

static void AEGIS_process_assoc_data(AEGIS_state* S, uint8_t* AD, uint64_t ADlen) {
    //
    // The AEGIS-128 function for processing associated data
    //
    // Input
    //
    // AD - the associated data vestor of length ADlen bytes
    //
    // Input/output
    //
    // S - the state of AEGIS-128 (the registers s0 .. s4 of length AES_BLOCKLEN)
    //
    uint64_t adlen = 8 * ADlen; // Length of padded associated data in bits
    if (adlen == 0) return; // If no associated data return without state update
    uint64_t i, j;
    uint64_t nblocks = (uint64_t) ceil((double) adlen / 128); // Number of blocks for padded associated data
    uint8_t curblock[AES_BLOCKLEN]; // The current associated data block
    uint8_t** padded_blocks = malloc(nblocks * sizeof (uint8_t));
    for (i = 0; i < nblocks; i++)
        padded_blocks[i] = malloc(AES_BLOCKLEN * sizeof (uint8_t));
    for (i = 0; i < nblocks; i++)
        for (j = 0; j < AES_BLOCKLEN; j++)
            padded_blocks[i][j] = 0;
    for (i = 0; i < ADlen; i++)
        padded_blocks[(uint64_t) (i / AES_BLOCKLEN)][i % AES_BLOCKLEN] = AD[i];
    // For each associated data block perform state update
    for (i = 0; i < nblocks; i++) {
        for (j = 0; j < AES_BLOCKLEN; j++)
            curblock[j] = padded_blocks[i][j];
        AEGIS_state_update(S, curblock);
    }
    // Free locally reserved memory
    for (i = 0; i < nblocks; i++)
        free(padded_blocks[i]);
    free(padded_blocks);
    return;
}

static void AEGIS_encrypt(AEGIS_state* S, uint8_t* C, uint8_t* P, uint64_t MSGlen) {
    //
    // The AEGIS-128 encryption function
    //
    // Input
    //
    // P - the plaintext of length MSGlen bytes
    //
    // Input/output
    //
    // S - the state of AEGIS-128 (the registers s0 .. s4 of length AES_BLOCKLEN)
    //
    // Output
    //
    // C - the ciphertext of length MSGlen bytes
    //
    uint64_t msglen = 8 * MSGlen; // Length of padded plaintext in bits
    if (msglen == 0) return; // If no plaintext return without state update
    uint64_t i, j;
    uint64_t byte_count; // Counter of produced ciphertext bytes 
    uint64_t nblocks = (uint64_t) ceil((double) msglen / 128); // Number of blocks for padded plaintext
    uint8_t P_curblock[AES_BLOCKLEN]; // The current plaintext block
    uint8_t C_curblock[AES_BLOCKLEN]; // The current ciphertext block
    uint8_t** padded_blocks = malloc(nblocks * sizeof (uint8_t));
    for (i = 0; i < nblocks; i++)
        padded_blocks[i] = malloc(AES_BLOCKLEN * sizeof (uint8_t));
    for (i = 0; i < nblocks; i++)
        for (j = 0; j < AES_BLOCKLEN; j++)
            padded_blocks[i][j] = 0;
    for (i = 0; i < MSGlen; i++)
        padded_blocks[(uint64_t) (i / AES_BLOCKLEN)][i % AES_BLOCKLEN] = P[i];
    // Perform encryption and state update
    byte_count = 0;
    for (i = 0; i < nblocks; i++) {
        for (j = 0; j < AES_BLOCKLEN; j++)
            P_curblock[j] = padded_blocks[i][j];
        for (j = 0; j < AES_BLOCKLEN; j++) {
            C_curblock[j] = P_curblock[j]^S->s1[j]^S->s4[j]^(S->s2[j] & S->s3[j]);
            if (byte_count < MSGlen) {
                C[byte_count] = C_curblock[j];
                byte_count++;
            }
        }
        AEGIS_state_update(S, P_curblock);
    }
    // Free locally reserved memory
    for (i = 0; i < nblocks; i++)
        free(padded_blocks[i]);
    free(padded_blocks);
    return;
}

static void AEGIS_decrypt(AEGIS_state* S, uint8_t* C, uint8_t* P, uint64_t MSGlen) {
    //
    // The AEGIS-128 decryption function
    //
    // Input
    //
    // C - the ciphertext of length MSGlen bytes
    //
    // Input/output
    //
    // S - the state of AEGIS-128 (the registers s0 .. s4 of length AES_BLOCKLEN)
    //
    // Output
    //
    // P - the plaintext of length MSGlen bytes
    //
    uint64_t msglen = 8 * MSGlen; // Length of padded plaintext in bits
    if (msglen == 0) return; // If no ciphertext return without state update
    uint64_t i, j;
    uint64_t byte_count; // Counter of produced plaintext bytes
    uint64_t nblocks = (uint64_t) ceil((double) msglen / 128); // Number of blocks for padded plaintext
    uint8_t P_curblock[AES_BLOCKLEN]; // The current plaintext block
    uint8_t C_curblock[AES_BLOCKLEN]; // The current ciphertext block
    uint8_t** padded_blocks = malloc(nblocks * sizeof (uint8_t));
    for (i = 0; i < nblocks; i++)
        padded_blocks[i] = malloc(AES_BLOCKLEN * sizeof (uint8_t));
    for (i = 0; i < nblocks; i++)
        for (j = 0; j < AES_BLOCKLEN; j++)
            padded_blocks[i][j] = 0;
    for (i = 0; i < MSGlen; i++)
        padded_blocks[(uint64_t) (i / AES_BLOCKLEN)][i % AES_BLOCKLEN] = C[i];
    // Perform decryption and state update
    byte_count = 0;
    for (i = 0; i < nblocks; i++) {
        for (j = 0; j < AES_BLOCKLEN; j++)
            C_curblock[j] = padded_blocks[i][j];
        for (j = 0; j < AES_BLOCKLEN; j++) {
            P_curblock[j] = C_curblock[j]^S->s1[j]^S->s4[j]^(S->s2[j] & S->s3[j]);
            if (byte_count < MSGlen) {
                P[byte_count] = P_curblock[j];
                byte_count++;
            }
        }
        AEGIS_state_update(S, P_curblock);
    }
    // Free locally reserved memory
    for (i = 0; i < nblocks; i++)
        free(padded_blocks[i]);
    free(padded_blocks);
    return;
}

static void AEGIS_finalize(AEGIS_state* S, uint8_t* Tprim, uint64_t adlen, uint64_t msglen) {
    //
    // The AEGIS-128 finalize function - produces the authentication tag Tprim
    //
    // Input
    //
    // adlen - length of associated data in bits
    // msglen - length of plaintext in bits
    //
    // Input/output
    //
    // S - the state of AEGIS-128 (the registers s0 .. s4 of length AES_BLOCKLEN)
    //
    // Output
    //
    // Tprim - the authentication tag of length AES_BLOCKLEN
    //
    int i;
    uint8_t tmp[AES_BLOCKLEN];
    uint8_t v[64];
    uint8_t b[8];
    tobinary(adlen, v, 64); // Convert adlen to a vector of bits
    tobytes(v, 64, b); // Then, convert this bit vector to a vector of bytes
    // Produce the first 8 bytes of the tmp vector
    for (i = 0; i < 8; i++)
        tmp[i] = b[7 - i];
    tobinary(msglen, v, 64); // Convert msglen to a vector of bits
    tobytes(v, 64, b); // Then, convert this bit vector to a vector of bytes
    // Produce the second 8 bytes of the tmp vector
    for (i = 8; i < 16; i++)
        tmp[i] = b[15 - i];
    // Update the tmp vector by XORing it with s3
    for (i = 0; i < AES_BLOCKLEN; i++)
        tmp[i] ^= S->s3[i];
    // Perform state update 7 times
    for (i = 0; i < 7; i++) {
        AEGIS_state_update(S, tmp);
    }
    // Produce Tprim as an XOR of s0 .. s4
    for (i = 0; i < AES_BLOCKLEN; i++)
        Tprim[i] = S->s0[i]^S->s1[i]^S->s2[i]^S->s3[i]^S->s4[i];
    return;
}

static void tobinary(uint64_t a, uint8_t* v, uint64_t n) {
    //
    // Returns the binary representation of a with n bits in the vector v
    //
    uint64_t i, j, r, a1;
    a1 = a;
    for (i = 0; i < n; i++)
        v[i] = 0;
    j = n - 1;
    do {
        r = a1 % 2;
        a1 = (uint64_t) (a1 / 2);
        v[j] = r;
        j--;
    } while (a1 > 0);
    return;
}

static void tobytes(uint8_t* v, uint64_t n, uint8_t* b) {
    //
    // Returns a byte vector b representing a binary vector v of length n = 8*q
    //
    uint64_t q = (uint64_t) n / 8;
    uint64_t i, j, k;
    for (i = 0; i < q; i++)
        b[i] = 0;
    for (i = 0; i < q; i++) {
        k = 7;
        for (j = i * 8; j < (i + 1)*8; j++) {
            if (v[j] == 1)
                b[i] |= 1 << k;
            k--;
        }
    }
    return;
}

void AEGIS_generation_encryption(uint8_t* key, uint8_t* IV, uint8_t* AD, uint64_t ADlen, uint8_t* P, uint8_t* C, uint64_t MSGlen, uint8_t* Tprim) {
    //
    // The generation-encryption function of AEGIS-128
    //
    // Input
    //
    // key - the key of length AES_KEYLEN bytes (16 bytes for AEGIS-128)
    // IV - the initialization vector of length AES_KEYLEN bytes (16 bytes for AEGIS-128)
    // AD - the associated data vestor of length ADlen bytes
    // P - the plaintext of length MSGlen bytes
    //
    // Output
    //
    // C - the ciphertext of length MSGlen bytes
    // Tprim - the authentication tag of length AES_BLOCKLEN
    //
    uint64_t adlen = ADlen * 8;
    uint64_t msglen = MSGlen * 8;
    AEGIS_state S;
    AEGIS_initialize(&S, key, IV);
    AEGIS_process_assoc_data(&S, AD, ADlen);
    AEGIS_encrypt(&S, C, P, MSGlen);
    AEGIS_finalize(&S, Tprim, adlen, msglen);
    return;
}

void AEGIS_decryption_verification(uint8_t* key, uint8_t* IV, uint8_t* AD, uint64_t ADlen, uint8_t* P, uint8_t* C, uint64_t MSGlen, uint8_t* Tprim) {
    //
    // The decryption-verification function of AEGIS-128
    //
    // Input
    //
    // key - the key of length AES_KEYLEN bytes (16 bytes for AEGIS-128)
    // IV - the initialization vector of length AES_KEYLEN bytes (16 bytes for AEGIS-128)
    // AD - the associated data vestor of length ADlen bytes
    // C - the ciphertext of length MSGlen bytes
    //
    // Output
    //
    // P - the plaintext of length MSGlen bytes
    // Tprim - the authentication tag of length AES_BLOCKLEN
    //
    uint64_t adlen = ADlen * 8;
    uint64_t msglen = MSGlen * 8;
    AEGIS_state S;
    AEGIS_initialize(&S, key, IV);
    AEGIS_process_assoc_data(&S, AD, ADlen);
    AEGIS_decrypt(&S, C, P, MSGlen);
    AEGIS_finalize(&S, Tprim, adlen, msglen);
    return;
}