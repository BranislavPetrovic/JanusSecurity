#include "ccm.h"
#include "aes.h"

uint8_t CCM_gen_encrypt(uint8_t* key, uint8_t* N, uint8_t n, uint8_t* A, ulong a, uint8_t* P, ulong p, uint8_t t, uint8_t* C, ulong c) {
    //
    // The generation-encryption function of CCM with AES (NIST 800-38C)
    //
    // Input
    //
    // key - the AES key of length AES_KEYLEN (in bytes)
    // N - the nonce of length n (in bytes)
    // A - the associated data of length a (in bytes)
    // P - the plaintext of length p (in bytes)
    // t - the length of MAC (in bytes)
    //
    // Output
    //
    // C - the ciphertext of length c (in bytes)
    //
    ulong i;
    uint8_t err = 0x00, member;
    ulong m = 1 + (p / AES_BLOCKLEN); // Length of the plaintext in blocks
    ulong na = 1 + (a / AES_BLOCKLEN); // Length of the associated data in blocks
    uint8_t* B; // The output buffer from the input data formatting procedure
    ulong b; // Length of the output buffer B in bytes
    uint8_t q = FIFTEEN - n; // According to the standard NIST 800-38c, n + q = 15
    // Check whether the values of t and n are permitted (according to NIST 800-38C)
    // Check t first
    uint8_t* tset = malloc((MAX_T + 1) * sizeof (uint8_t));
    for (i = 0; i < (MAX_T + 1); i++)
        tset[i] = 0;
    tset[4] = tset[6] = tset[8] = tset[10] = tset[12] = tset[14] = tset[16] = 1;
    member = ismember(tset, (MAX_T + 1), (ulong) t);
    if (member == 0) {
        err = 1;
        free(tset);
        return err; // Bad value of t, so return with error code 1
    }
    free(tset);
    // Next, check n
    uint8_t* nset = malloc((MAX_N + 1) * sizeof (uint8_t));
    for (i = 0; i < (MAX_N + 1); i++)
        nset[i] = 0;
    nset[7] = nset[8] = nset[9] = nset[10] = nset[11] = nset[12] = nset[13] = 1;
    member = ismember(nset, (MAX_N + 1), (ulong) n);
    if (member == 0) {
        err = 2;
        free(nset);
        free(tset);
        return err; // Bad value of n, so return with error code 2
    }
    free(nset);
    if (a > 0) { // There is associated data
        b = (m + na + 1) * AES_BLOCKLEN;
        B = malloc(b * sizeof (uint8_t));
    } else { // There is no associated data
        b = (m + 1) * AES_BLOCKLEN;
        B = malloc(b * sizeof (uint8_t));
    }
    uint8_t* T = malloc(t * sizeof (uint8_t));
    // Calling the formatting procedure
    cbcformat(N, n, A, a, P, p, t, B, b);
    // Generating the initial counter block Ctr0
    uint8_t* Ctr0 = malloc(AES_BLOCKLEN * sizeof (uint8_t));
    initctrblock(N, n, Ctr0);
    // Producing the MAC
    cbcmac(key, B, b, T, t);
    // Generating the encryption stream of bytes
    ulong s = AES_BLOCKLEN * m;
    uint8_t* S_stream = malloc(s * sizeof (uint8_t));
    uint8_t S0[AES_BLOCKLEN];
    genSstream(key, Ctr0, q, m, S_stream, s, S0);
    // Computing the final CCM output
    for (i = 0; i < p; i++)
        C[i] = P[i]^S_stream[i];
    for (i = p; i < p + t; i++)
        C[i] = T[i - p]^S0[i - p];
    // Free the memory allocated in this context
    free(B);
    free(T);
    free(Ctr0);
    free(S_stream);
    return err;
}

uint8_t CCM_decrypt_verify(uint8_t* key, uint8_t* C, ulong c, uint8_t t, uint8_t* N, uint8_t n, uint8_t* A, ulong a, uint8_t* P, ulong p) {
    //
    // The decryption-verification function of CCM with AES (NIST 800-38C)
    //
    // Input
    //
    // key - the AES key of length AES_KEYLEN (in bytes)
    // C - the ciphertext of length c (in bytes)
    // t - the length of MAC (in bytes)
    // N - the nonce of length n (in bytes)
    // A - the associated data of length a (in bytes)
    //
    // Output
    //
    // P - the plaintext of length p (in bytes)
    //
    uint8_t invalid = 0;
    // Check the length c of the ciphertext
    if (c < t) {
        invalid = 1;
        return invalid;
    }
    ulong i, j;
    ulong m = 1 + (p / AES_BLOCKLEN); // Length of the plaintext in blocks
    ulong na = 1 + (a / AES_BLOCKLEN); // Length of the associated data in blocks
    uint8_t* B; // The output buffer from the input data formatting procedure
    ulong b; // Length of the output buffer B in bytes
    uint8_t q = FIFTEEN - n;
    if (a > 0) { // There is associated data
        b = (m + na + 1) * AES_BLOCKLEN;
        B = malloc(b * sizeof (uint8_t));
    } else { // There is no associated data
        b = (m + 1) * AES_BLOCKLEN;
        B = malloc(b * sizeof (uint8_t));
    }
    uint8_t* T = malloc(t * sizeof (uint8_t));
    uint8_t* T1 = malloc(t * sizeof (uint8_t));
    // Generating the initial counter block Ctr0
    uint8_t* Ctr0 = malloc(AES_BLOCKLEN * sizeof (uint8_t));
    initctrblock(N, n, Ctr0);
    // Generating the encryption stream of bytes
    ulong s = AES_BLOCKLEN * m;
    uint8_t* S_stream = malloc(s * sizeof (uint8_t));
    uint8_t S0[AES_BLOCKLEN];
    genSstream(key, Ctr0, q, m, S_stream, s, S0);
    // Decrypt the plaintext
    for (i = 0; i < p; i++)
        P[i] = C[i]^S_stream[i];
    for (i = 0; i < t; i++)
        T1[i] = C[i + p]^S0[i];
    // Calling the formatting procedure
    cbcformat(N, n, A, a, P, p, t, B, b);
    // Producing the MAC
    cbcmac(key, B, b, T, t);
    // Check the validity of the obtained MAC
    for (i = 0; i < t; i++) {
        if (T[i] != T1[i]) {
            for (j = 0; j < p; j++)
                P[i] = 0;
            invalid = 1; // Authentication failure
        }
    }
    // Free the memory allocated in this context
    free(B);
    free(T);
    free(T1);
    free(Ctr0);
    free(S_stream);
    return invalid;
}

void cbcformat(uint8_t* N, uint8_t n, uint8_t* A, ulong a, uint8_t* P, ulong p, uint8_t t, uint8_t* B, ulong b) {
    //
    // Formatting procedure to prepare data for CCM with AES (According to NIST 800-38C)
    //
    // Input
    //
    // N - the nonce of length n (in bytes)
    // A - the associated data of length a (in bytes); if a = 0, then no associated data is used
    // P - the plaintext of length p (in bytes)
    // t - the length of the MAC tag (in bytes)
    // b - the length of the output buffer B (in bytes)
    //
    // Output
    //
    // B - the buffer ready to be processed by the CCM of length b (in bytes)
    //
    ulong i;
    // n + q = 15, so compute q (the size in bytes of the representation of the size p of P in bytes)
    uint8_t q = FIFTEEN - n;
    ulong nbits; // Length in bits of the representation of a
    ulong nabytes; // nbits / 8, length in bytes of the representation of a
    for (i = 0; i < b; i++) // Zero-out the output buffer B
        B[i] = 0;
    // Produce the flags byte in the block B0
    uint8_t flags = 0;
    if (a > 0)
        flags = 0x40;
    uint8_t tt = (uint8_t) (t - 2) / 2;
    flags |= (tt << 3);
    uint8_t qq = q - 1;
    flags |= qq;
    // Producing of the block B0
    B[0] = flags; // The 0-th byte of the block B0 is the flags byte
    for (i = 0; i < n; i++) // The flags byte is followed by the nonce
        B[i + 1] = N[i];
    // Computing Q - the binary encoding of the length of P in bytes (Q is a byte vector)
    uint8_t* v = malloc(8 * q * sizeof (uint8_t));
    tobinary(p, v, 8 * q);
    uint8_t* Q = malloc(q * sizeof (uint8_t));
    tobytes(v, 8 * q, Q);
    for (i = 0; i < q; i++) // After the nonce, the rest of the block B0 is filled with the bytes of Q
        B[i + n + 1] = Q[i];
    // Producing of the block B1 and the rest of the blocks containing associated data (if any)
    if (a > 0) {
        if (a < ALIMIT_1)
            nbits = 16;
        else if ((a >= ALIMIT_1)&&(a < ALIMIT_2))
            nbits = 48;
        else
            nbits = 80;
        // Computing the byte representation of a
        uint8_t* v1 = malloc(nbits * sizeof (uint8_t));
        uint8_t* abytes = malloc((nbits / 8) * sizeof (uint8_t));
        tobinary(a, v1, nbits);
        tobytes(v1, nbits, abytes);
        if (nbits == 48) {
            abytes[0] = 0xff;
            abytes[1] = 0xfe;
        } else if (nbits == 80) {
            abytes[0] = 0xff;
            abytes[1] = 0xff;
        }
        nabytes = (ulong) nbits / 8; // Number of bytes in the byte representation of a
        for (i = 0; i < nabytes; i++) // The byte representation of a is the first part of the block B1
            B[AES_BLOCKLEN + i] = abytes[i];
        for (i = 0; i < a; i++) // After the byte representation of a, the rest of the block B1 and (eventually) more blocks are filled with the bytes of A
            B[AES_BLOCKLEN + nabytes + i] = A[i];
        // Free the memory allocated in this context
        free(v1);
        free(abytes);
    }
    ulong nfullblocks = 1 + (ulong) ((AES_BLOCKLEN + nabytes + a) / AES_BLOCKLEN); // The number of full blocks after completing the placement of A in the buffer B
    for (i = nfullblocks * AES_BLOCKLEN; i < p + nfullblocks * AES_BLOCKLEN; i++) // The plaintext block(s) follow and this completes the formatting procedure
        B[i] = P[i - nfullblocks * AES_BLOCKLEN];
    // Free the rest of the memory allocated in this function
    free(v);
    free(Q);
    return;
}

void initctrblock(uint8_t* N, uint8_t n, uint8_t* Ctr0) {
    //
    // Generating the initial counter block Ctr0 for CCM with AES (According to NIST 800-38C)
    //
    // Input
    //
    // N - the nonce of length n (in bytes)
    //
    // Output
    //
    // The initial counter block Ctr0 of length AES_BLOCKLEN (128/192/256 bytes)
    //
    uint8_t q = FIFTEEN - n;
    ulong i;
    for (i = 0; i < AES_BLOCKLEN; i++)
        Ctr0[i] = 0;
    // Computing the flags byte (byte 0 of Ctr0)
    uint8_t flags = q - 1;
    Ctr0[0] = flags;
    // The flags byte is followed by the nonce (n bytes) and the rest of the bytes of Ctr0 are 0 (q bytes)
    for (i = 0; i < n; i++)
        Ctr0[i + 1] = N[i];
    return;
}

static uint8_t ismember(uint8_t* a, ulong n, ulong e) {
    //
    // Check whether e belongs to the set a
    // n is the length of a, which is equal to the maximum element of the set
    // if e is element of the set, then a[e] = 1
    //
    // Returns 1 if e is a member of the set
    //
    uint8_t member = 0;
    ulong i;
    for (i = 0; i < n; i++) {
        if (a[e] == 1) {
            member = 1;
            break;
        }
    }
    return member;
}

void cbcmac(uint8_t *key, uint8_t* B, ulong b, uint8_t* T, uint8_t t) {
    //
    // Producing CBCMAC for CCM with AES (NIST 800-38C)
    //
    // Input:
    //
    // key - the AES key of length AES_KEYLEN
    //
    // B - the input buffer of length b (in bytes) produced by means of the CCM formatting function
    // b must be a multiple of AES_BLOCKLEN, the length of the AES block
    //
    // t - the length of the MAC tag T (in bytes, t >= 8 is recommended)
    //
    // Output: 
    //
    // T - the MAC tag produced in this function
    //
    int i, j;
    int r1 = (int) b / AES_BLOCKLEN; // r1 is r+1 (NIST 800-38C)
    struct AES_ctx ctx;
    // Produce Y_0 = E(K,B_0)
    uint8_t in[AES_BLOCKLEN]; // In this case, in is B_0
    for (i = 0; i < AES_BLOCKLEN; i++)
        in[i] = B[i];
    // Run the AES cipher to produce Y_0
    AES_init_ctx(&ctx, key);
    AES_ECB_encrypt(&ctx, in); // in is now the output Y_0 of AES
    // Produce Y_1, ..., Y_r; Y_i = E(K, B_i ^ Y_(i-1)), i = 1, ..., r
    for (i = 1; i < r1; i++) {
        // XOR the previous output block Y_(i-1) with the current input block B_i 
        // in is now B_i ^ Y_(i-1)
        for (j = 0; j < AES_BLOCKLEN; j++)
            in[j] ^= B[i * AES_BLOCKLEN + j];
        // Run the AES cipher to produce Y_i
        AES_init_ctx(&ctx, key);
        AES_ECB_encrypt(&ctx, in); // in is now the output Y_i of AES
    }
    // Compute the MAC - the first t bytes of Y_r
    for (i = 0; i < t; i++)
        T[i] = in[i];
    return;
}

static void genS(uint8_t *key, uint8_t* ctr, uint8_t* S) {
    //
    // Producing the block S_i for encipherment in CCM with AES128 (NIST 800-38C)
    //
    // Input:
    //
    // key - the AES key (the length of the key (in bytes) is AES_KEYLEN)
    //
    // ctr - the current value ctr_i of the counter (the length of the ctr (in bytes) is AES_BLOCKLEN)
    //
    // Output: 
    //
    // S - the current block S_i used for encipherment of the corresponding portion of the plaintext/MAC
    //
    int i, j;
    struct AES_ctx ctx;
    // Produce the output block S = E(K,ctr)
    uint8_t in[AES_BLOCKLEN]; // In this case, in is the value of the counter
    for (i = 0; i < AES_BLOCKLEN; i++)
        in[i] = ctr[i];
    // Run the AES cipher to produce S
    AES_init_ctx(&ctx, key);
    AES_ECB_encrypt(&ctx, in); // in is now the output Y_0 of AES128
    for (i = 0; i < AES_BLOCKLEN; i++)
        S[i] = in[i];
    return;
}

static void genctrvals(uint8_t** ctrarr, uint8_t q, ulong m) {
    // Generating counters for CCM and placing them in a byte array with m rows and q columns
    ulong bitmask = 0xff;
    long counterval = -1;
    ulong counterval1;
    int i, j;
    for (i = 0; i < m + 1; i++) {
        counterval++;
        counterval1 = counterval;
        for (j = q - 1; j >= 0; j--) {
            ctrarr[i][j] = counterval1 & bitmask;
            counterval1 = counterval1 >> 8;
        }
    }
    return;
}

void genSstream(uint8_t* key, uint8_t* Ctr0, uint8_t q, ulong m, uint8_t* S_stream, ulong s, uint8_t* S0) {
    //
    // Generating the encryption stream of bytes for CCM
    //
    // Input
    //
    // q - the size (in bytes) of the representation of the size p (in bytes) of the plaintext P
    // m - the number of blocks of length AES_BLOCKLEN to generate for S_stream
    // Ctr0 - the initial value of the counter (from initctrblock)
    //
    // Output
    //
    // S_stream - the stream of bytes (of length s bytes) of the running key used for encryption of the plaintext P in CCM
    // S0 - the block of bytes used for encryption of CBCMAC in CCM
    //
    int i, j;
    uint8_t S[AES_BLOCKLEN];
    uint8_t** ctrarr = malloc((m + 1) * sizeof (uint8_t*));
    for (i = 0; i < m + 1; i++)
        ctrarr[i] = malloc(q * sizeof (uint8_t));
    genctrvals(ctrarr, q, m);
    uint8_t* ctr = malloc(AES_BLOCKLEN * sizeof (uint8_t));
    for (i = 0; i < AES_BLOCKLEN; i++)
        ctr[i] = Ctr0[i];
    for (i = 0; i < m + 1; i++) {
        for (j = AES_BLOCKLEN - q; j < AES_BLOCKLEN; j++)
            ctr[j] = ctrarr[i][j - AES_BLOCKLEN + q];
        genS(key, ctr, S);
        if (i > 0) {
            for (j = 0; j < AES_BLOCKLEN; j++)
                S_stream[(i - 1) * AES_BLOCKLEN + j] = S[j];
        } else {
            for (j = 0; j < AES_BLOCKLEN; j++)
                S0[j] = S[j];
        }
    }
    // Free the memory allocated in this function
    for (i = 0; i < m + 1; i++)
        free(ctrarr[i]);
    free(ctrarr);
    free(ctr);
    return;
}

static void tobinary(ulong a, uint8_t* v, ulong n) {
    //
    // Returns the binary representation of a with n bits in the vector v
    //
    ulong i, j, r, a1;
    a1 = a;
    for (i = 0; i < n; i++)
        v[i] = 0;
    j = n - 1;
    do {
        r = a1 % 2;
        a1 = (ulong) (a1 / 2);
        v[j] = r;
        j--;
    } while (a1 > 0);
    return;
}

static void tobytes(uint8_t* v, ulong n, uint8_t* b) {
    //
    // Returns a byte vector b representing a binary vector v of length n = 8*q
    //
    ulong q = (ulong) n / 8;
    ulong i, j, k;
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