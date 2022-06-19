//*************************************************************************
// JANUS is a simple, robust, open standard signalling method for         *
// underwater communications. See <http://www.januswiki.org> for details. *
//*************************************************************************
// Example software implementations provided by STO CMRE are subject to   *
// Copyright (C) 2008-2018 STO Centre for Maritime Research and           *
// Experimentation (CMRE)                                                 *
//                                                                        *
// This is free software: you can redistribute it and/or modify it        *
// under the terms of the GNU General Public License version 3 as         *
// published by the Free Software Foundation.                             *
//                                                                        *
// This program is distributed in the hope that it will be useful, but    *
// WITHOUT ANY WARRANTY; without even the implied warranty of FITNESS     *
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for       *
// more details.                                                          *
//                                                                        *
// You should have received a copy of the GNU General Public License      *
// along with this program. If not, see <http://www.gnu.org/licenses/>.   *
//*************************************************************************
// Authors: Ricardo Martins, Luigi Elia D'Amaro, Giovanni Zappa           *
//*************************************************************************

// ISO C headers.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>

// JANUS headers.
#include <janus/janus.h>
#include <janus/defaults.h>
#include <janus/RC5_Janus.h>
#include <janus/aes.h>
#include <janus/ccm.h>
#include <janus/aegis_256.h>

// Local headers.
#include "options.h"

#ifdef CLI_TX
#define PROG_NAME "janus-tx"
#endif

#ifdef CLI_RX
#define PROG_NAME "janus-rx"
#endif

static int done = 0;

static void
signal_handler_callback(int signum) {
    done = 1;
}

static void
signal_handler_init() {
#ifdef CLI_RX
    // Register signal and signal handler.
    signal(SIGABRT, signal_handler_callback);
    signal(SIGFPE, signal_handler_callback);
    signal(SIGILL, signal_handler_callback);
    signal(SIGINT, signal_handler_callback);
    signal(SIGSEGV, signal_handler_callback);
    signal(SIGTERM, signal_handler_callback);
#if !(defined(_WIN32) || defined(__WIN32__))
    signal(SIGHUP, signal_handler_callback);
    signal(SIGPIPE, signal_handler_callback);
    signal(SIGQUIT, signal_handler_callback);
#endif
#endif
}

int
main(int argc, char** argv) {
    cli_options_t cli_options = 0;
    janus_parameters_t params = 0;

    cli_options = cli_options_new(argc, argv);
    if (!cli_options_is_valid(cli_options)) {
        fprintf(stderr, "Usage: " PROG_NAME " [OPTIONS]\n");
        fprintf(stderr, "JANUS is a simple, robust signaling method for underwater communications.\n\n");
        cli_options_usage(cli_options);
        fprintf(stderr, "\nCopyright (C) 2008-2018 STO Centre for Maritime Research and Experimentation (CMRE)\n");
        cli_options_free(cli_options);
        return 1;
    }

    signal_handler_init();

    // Parameters.
    params = janus_parameters_new();
    cli_options_get_params(cli_options, params);
    if (params->verbose > 1)
        cli_options_dump(cli_options);

#ifdef CLI_TX
    {
        janus_simple_tx_t simple_tx = 0;
        janus_packet_t packet = 0;
        janus_tx_state_t state = 0;

        // Initialize simple tx.
        simple_tx = janus_simple_tx_new(params);
        if (!simple_tx) {
            fprintf(stderr, "ERROR: failed to initialize transmitter.\n");
            janus_parameters_free(params);
            cli_options_free(cli_options);
            return 1;
        }

        // Initialize packet.
        packet = janus_packet_new(params->verbose);
        if (cli_options_get_packet(cli_options, packet) != JANUS_ERROR_NONE) {
            fprintf(stderr, "ERROR: failed to initialize packet.\n");
            janus_packet_free(packet);
            janus_simple_tx_free(simple_tx);
            janus_parameters_free(params);
            cli_options_free(cli_options);
            return 1;
        }

        janus_uint8_t* bytes = janus_packet_get_bytes(packet);
        //bytes[7] = janus_packet_get_crc(packet);
        janus_uint8_t adb[4];
//        uint8_t key[] = {0x91, 0x5f, 0x46, 0x19, 0xbe, 0x41, 0xb2, 0x51, 0x63, 0x55, 0xa5, 0x01, 0x10, 0xa9, 0xce, 0x91};
//        uint32_t inblock, outblock;
        int i;
        FILE* fp;
        fp = fopen("/home/ttm4128/teglasyiffrc5.txt", "wb");
        janus_uint8_t bin[JANUS_MIN_PKT_SIZE * 8];
        clock_t encStart, encEnd;
        double cpu_time_used;
        double avgEnc, avgDec = 0;

//        printf("ADB: ");
//        for (i = 0; i < 8; i++) {
//            if (i > 2 && i < 7) {
//                adb[i - 3] = bytes[i];
//                printf("%02x ", adb[i - 3]);
//            }
//
//            //*bytes++;
//        }
//
//        for (i = 0; i < 4; i++) {
//            inblock += adb[i] << (24 - 8 * i);
//        }
//
//        printf("\nkey = ");
//        for (i = 0; i < 16; i++) printf("%.2X ", key[i]);
//        for (i = 0; i < 100000; i++) {
//            encStart = clock();
//            outblock = Janus_RC5_encrypt(inblock, key);
//            encEnd = clock();
//            cpu_time_used = ((double) (encEnd - encStart)) / CLOCKS_PER_SEC;
//            avgEnc += cpu_time_used;
//        }
//
//        avgEnc /= 100000;
//        printf("\n\nplaintext %.8lX ---> ciphertext %.8lX, average seconds taken: %.10f \n", inblock, outblock, avgEnc);
//
//        for (i = 0; i < 4; i++) {
//            bytes[i + 3] = (janus_uint8_t) (outblock >> (24 - 8 * i));
//        }
//
//        for (i = 0; i < 100000; i++) {
//            encStart = clock();
//            inblock = Janus_RC5_decrypt(outblock, key);
//            encEnd = clock();
//            cpu_time_used = ((double) (encEnd - encStart)) / CLOCKS_PER_SEC;
//            avgDec += cpu_time_used;
//        }
//
//        avgDec /= 100000;
//        printf("\n\nciphertext %.8lX ---> plaintext %.8lX, average seconds taken: %.10f \n", outblock, inblock, avgDec);
//
//        // Initialize state.
//        state = janus_tx_state_new((params->verbose > 1));
//
//        // Transmit.
//        janus_simple_tx_execute(simple_tx, packet, state);
//
//        janus_utils_dec2bin(bytes, JANUS_MIN_PKT_SIZE, bin);
//        for (i = 0; i < 64; i++)
//            fprintf(fp, "%u", bin[i]);
//        fclose(fp);

        // --- CCM, no cargo ---
//        janus_packet_t ccmpkt1 = 0;
//        janus_packet_t ccmpkt2 = 0;
//        janus_packet_t ccmpkt3 = 0;
//        janus_packet_t ccmpkt4 = 0;
//        janus_packet_t ccmpkt5 = 0;
//        ccmpkt1 = janus_packet_new(params->verbose);
//        janus_uint8_t* ccmpkt1bytes = janus_packet_get_bytes(ccmpkt1);
//        janus_utils_dec2bin(ccmpkt1bytes, JANUS_MIN_PKT_SIZE, bin);
//        fp = fopen("/home/ttm4128/ccmnocargopkt1.txt", "wb");
//        uint8_t ccmpkt1adbbin[34];
//
//        uint8_t N[] = {0x00, 0x00, 0x00, 0x03};
//        uint8_t Ndouble[] = {0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03};
//        uint8_t Nbin[32];
//        uint8_t key[] = {0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf};
//        uint8_t A[3];
//        uint8_t Abin[24];
//        uint8_t n = 8;
//        ulong a = 3;
//        ulong p = 12;
//        uint8_t t = 4;
//        //int Tlen = 1;
//        uint8_t err;
//        ulong c = p + t;
//        uint8_t* C = malloc(c * sizeof (uint8_t));
//        uint8_t cBin[8 * c];
//        //cBin[0] = "\0";
//        uint8_t TSCD[] = {0x01, 0x02, 0x03, 0x04};
//        uint8_t MMSIA[] = {0x05, 0x06, 0x07, 0x08};
//        uint8_t MMSIB[] = {0x09, 0x10, 0x11, 0x12};
//        uint8_t MMSIAbin[32];
//        uint8_t MMSIBbin[32];
//        uint8_t P[12];
//
//        janus_utils_dec2bin(MMSIA, 4, MMSIAbin);
//        janus_utils_dec2bin(MMSIB, 4, MMSIBbin);
//        MMSIAbin[30] = MMSIAbin[31] = 0;
//        MMSIBbin[30] = MMSIBbin[31] = 0;
//        janus_utils_bin2dec(MMSIAbin, 32, MMSIA);
//        janus_utils_bin2dec(MMSIBbin, 32, MMSIB);
//
//        for (i = 0; i < 4; i++) {
//            P[i] = TSCD[i];
//            P[i + 4] = MMSIA[i];
//            P[i + 8] = MMSIB[i];
//        }
//
//        for (i = 0; i < 64; i++) {
//            if (i >= 0 && i <= 21) {
//                Abin[i] = bin[i];
//            } else if (i >= 54 && i <= 55) {
//                Abin[i - 32] = bin[i];
//            }
//        }
//
//        janus_utils_bin2dec(Abin, 24, A);
//
//        printf("The key\n\n");
//        for (i = 0; i < AES_KEYLEN; i++)
//            printf("%02x ", key[i]);
//
//        printf("\n\n");
//
//        printf("\n\n");
//        printf("The nonce N\n\n");
//        for (i = 0; i < n; i++)
//            printf("%02x ", Ndouble[i]);
//
//        printf("\n\n");
//
//        printf("\n\n");
//        printf("The associated data A\n\n");
//        for (i = 0; i < a; i++) {
//            printf("%02x ", A[i]);
//            if ((i + 1) % 16 == 0)
//                printf("\n");
//        }
//
//        printf("\n\n");
//
//        printf("\n\n");
//        printf("The plaintext P\n\n");
//        for (i = 0; i < p; i++) {
//            printf("%02x ", P[i]);
//            if ((i + 1) % 16 == 0)
//                printf("\n");
//        }
//
//        printf("\n\n");
//
//        printf("\n\n");
//        printf("The length of the MAC tag T is t = %d\n\n", t);
//        
//        err = CCM_gen_encrypt(key, Ndouble, n, A, a, P, p, t, C, c);
//
////        for (i = 0; i < 100000; i++) {
////            encStart = clock();
////            err = CCM_gen_encrypt(key, Ndouble, n, A, a, P, p, t, C, c);
////            encEnd = clock();
////            cpu_time_used = ((double) (encEnd - encStart)) / CLOCKS_PER_SEC;
////            avgEnc += cpu_time_used;
////        }
////
////        avgEnc /= 100000;
////        printf("\n\nDid gen-enc 100000 times, average seconds taken: %.10f \n", avgEnc);
//
////        for (i = 0; i < 100000; i++) {
////            encStart = clock();
////            err = CCM_decrypt_verify(key, C, c, t, Ndouble, n, A, a, P, p);
////            encEnd = clock();
////            cpu_time_used = ((double) (encEnd - encStart)) / CLOCKS_PER_SEC;
////            avgDec += cpu_time_used;
////        }
////
////        avgDec /= 100000;
////        printf("\n\nDid dec-ver 100000 times, average seconds taken: %.10f \n", avgDec);
//
//        if (err == 0) {
//            printf("The authenticated and encrypted output C\n\n");
//            for (i = 0; i < p + t; i++) {
//                printf("%02x ", C[i]);
//
//                if ((i + 1) % AES_BLOCKLEN == 0)
//                    printf("\n");
//            }
//            printf("\n\n");
//
//        } else {
//            printf("Error in input data, error code = %d", err);
//        }
//
//        printf("\n\n");
//
//        // --- packet 1 ---        
//        janus_utils_dec2bin(N, 4, Nbin);
//
//        for (i = 0; i < 32; i++) {
//            bin[i + 22] = Nbin[i];
//        }
//
//        janus_utils_bin2dec(bin, 64, ccmpkt1bytes);
//        janus_packet_set_bytes(ccmpkt1, ccmpkt1bytes);
//
//        // Initialize state.
//        state = janus_tx_state_new((params->verbose > 1));
//
//        // Transmit.
//        janus_simple_tx_execute(simple_tx, ccmpkt1, state);
//
//        janus_utils_dec2bin(ccmpkt1bytes, JANUS_MIN_PKT_SIZE, bin);
//
//        for (i = 0; i < 64; i++)
//            fprintf(fp, "%u", bin[i]);
//        fclose(fp);
//
//        // --- packet 2 ---
//        fp = fopen("/home/ttm4128/ccmnocargopkt2.txt", "wb");
//        janus_utils_dec2bin(C, 16, cBin);
//
//        ccmpkt2 = janus_packet_new(params->verbose);
//        janus_uint8_t* ccmpkt2bytes = janus_packet_get_bytes(ccmpkt2);
//        janus_utils_dec2bin(ccmpkt2bytes, JANUS_MIN_PKT_SIZE, bin);
//
//        for (i = 0; i < 32; i++) {
//            bin[i + 22] = cBin[i];
//        }
//
//        janus_utils_bin2dec(bin, 64, ccmpkt2bytes);
//        janus_packet_set_bytes(ccmpkt2, ccmpkt2bytes);
//
//        // Initialize state.
//        state = janus_tx_state_new((params->verbose > 1));
//
//        // Transmit.
//        janus_simple_tx_execute(simple_tx, ccmpkt2, state);
//
//        janus_utils_dec2bin(ccmpkt2bytes, JANUS_MIN_PKT_SIZE, bin);
//
//        for (i = 0; i < 64; i++)
//            fprintf(fp, "%u", bin[i]);
//        fclose(fp);
//
//        // --- packet 3 ---
//        fp = fopen("/home/ttm4128/ccmnocargopkt3.txt", "wb");
//        ccmpkt3 = janus_packet_new(params->verbose);
//        janus_uint8_t* ccmpkt3bytes = janus_packet_get_bytes(ccmpkt3);
//        janus_utils_dec2bin(ccmpkt3bytes, JANUS_MIN_PKT_SIZE, bin);
//
//        for (i = 0; i < 32; i++) {
//            bin[i + 22] = cBin[i + 32];
//        }
//
//        janus_utils_bin2dec(bin, 64, ccmpkt3bytes);
//        janus_packet_set_bytes(ccmpkt3, ccmpkt3bytes);
//
//        // Initialize state.
//        state = janus_tx_state_new((params->verbose > 1));
//
//        // Transmit.
//        janus_simple_tx_execute(simple_tx, ccmpkt3, state);
//
//        janus_utils_dec2bin(ccmpkt3bytes, JANUS_MIN_PKT_SIZE, bin);
//
//        for (i = 0; i < 64; i++)
//            fprintf(fp, "%u", bin[i]);
//        fclose(fp);
//
//        // --- packet 4 ---
//        fp = fopen("/home/ttm4128/ccmnocargopkt4.txt", "wb");
//        ccmpkt4 = janus_packet_new(params->verbose);
//        janus_uint8_t* ccmpkt4bytes = janus_packet_get_bytes(ccmpkt4);
//        janus_utils_dec2bin(ccmpkt4bytes, JANUS_MIN_PKT_SIZE, bin);
//
//        for (i = 0; i < 32; i++) {
//            bin[i + 22] = cBin[i + 64];
//        }
//
//        janus_utils_bin2dec(bin, 64, ccmpkt4bytes);
//        janus_packet_set_bytes(ccmpkt4, ccmpkt4bytes);
//
//        // Initialize state.
//        state = janus_tx_state_new((params->verbose > 1));
//
//        // Transmit.
//        janus_simple_tx_execute(simple_tx, ccmpkt4, state);
//
//        janus_utils_dec2bin(ccmpkt4bytes, JANUS_MIN_PKT_SIZE, bin);
//
//        for (i = 0; i < 64; i++)
//            fprintf(fp, "%u", bin[i]);
//        fclose(fp);
//
//        // --- packet 5 ---
//        fp = fopen("/home/ttm4128/ccmnocargopkt5.txt", "wb");
//        ccmpkt5 = janus_packet_new(params->verbose);
//        janus_uint8_t* ccmpkt5bytes = janus_packet_get_bytes(ccmpkt5);
//        janus_utils_dec2bin(ccmpkt5bytes, JANUS_MIN_PKT_SIZE, bin);
//
//        for (i = 0; i < 32; i++) {
//            bin[i + 22] = cBin[i + 96];
//        }
//
//        janus_utils_bin2dec(bin, 64, ccmpkt5bytes);
//        janus_packet_set_bytes(ccmpkt5, ccmpkt5bytes);
//
//        // Initialize state.
//        state = janus_tx_state_new((params->verbose > 1));
//
//        // Transmit.
//        janus_simple_tx_execute(simple_tx, ccmpkt5, state);
//
//        janus_utils_dec2bin(ccmpkt5bytes, JANUS_MIN_PKT_SIZE, bin);
//
//        for (i = 0; i < 64; i++)
//            fprintf(fp, "%u", bin[i]);
//        fclose(fp);

        // CCM, cargo
        //janus_packet_t pkt = janus_packet_new(params->verbose);
        //        janus_uint8_t* bytes1 = janus_packet_get_bytes(pkt);
        //        janus_utils_dec2bin(bytes1, JANUS_MIN_PKT_SIZE, bin);
        //        janus_uint8_t* cargo[132];
        //        uint8_t N[] = {0x00, 0x00, 0x00, 0x03};
        //        uint8_t Nbin[32];
        //        uint8_t Nbl[26];
        //        uint8_t Ndouble[] = {0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03};
        //        uint8_t key[] = {0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf};
        //        uint8_t A[4];
        //        uint8_t TSCD[] = {0x01, 0x02, 0x03, 0x04};
        //        uint8_t MMSIA[] = {0x05, 0x06, 0x07, 0x08};
        //        uint8_t MMSIB[] = {0x09, 0x10, 0x11, 0x12};
        //        uint8_t n = 8;
        //        ulong a = 4;
        //        ulong p = 12;
        //        uint8_t t = 4;

        //        janus_utils_dec2bin(N, 4, Nbin);
        //        
        //        for(i = 0; i < 26; i++) {
        //            Nbl[i] = Nbin[i];
        //        }

        // AEGIS, no cargo
        janus_packet_t aegispkt1 = 0;
        janus_packet_t aegispkt2 = 0;
        janus_packet_t aegispkt3 = 0;
        janus_packet_t aegispkt4 = 0;

        aegispkt1 = janus_packet_new(params->verbose);
        janus_uint8_t* aegispkt1bytes = janus_packet_get_bytes(aegispkt1);
        janus_utils_dec2bin(aegispkt1bytes, JANUS_MIN_PKT_SIZE, bin);
        fp = fopen("/home/ttm4128/aegisnocargopkt1.txt", "wb");

        uint8_t IV[] = {0x00, 0x00, 0x00, 0x03};
        uint8_t IVwhole[] = {0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03,
            0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03,
            0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03};
        uint8_t IVbin[32];
        uint8_t key[] = {0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8,
            0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4,
            0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf};
        uint8_t A[3];
        uint8_t Abin[24];
        ulong adlen = 3;
        ulong msglen = 12;
        uint8_t Tprim[AES_BLOCKLEN];
        uint8_t* C = malloc(msglen * sizeof (uint8_t));
        uint8_t cBin[8 * msglen];
        uint8_t TSCD[] = {0x01, 0x02, 0x03, 0x04};
        uint8_t MMSIA[] = {0x05, 0x06, 0x07, 0x08};
        uint8_t MMSIB[] = {0x09, 0x10, 0x11, 0x12};
        uint8_t MMSIAbin[32];
        uint8_t MMSIBbin[32];
        uint8_t P[12];

        janus_utils_dec2bin(MMSIA, 4, MMSIAbin);
        janus_utils_dec2bin(MMSIB, 4, MMSIBbin);
        MMSIAbin[30] = MMSIAbin[31] = 0;
        MMSIBbin[30] = MMSIBbin[31] = 0;
        janus_utils_bin2dec(MMSIAbin, 32, MMSIA);
        janus_utils_bin2dec(MMSIBbin, 32, MMSIB);

        for (i = 0; i < 4; i++) {
            P[i] = TSCD[i];
            P[i + 4] = MMSIA[i];
            P[i + 8] = MMSIB[i];
        }

        for (i = 0; i < 64; i++) {
            if (i >= 0 && i <= 21) {
                Abin[i] = bin[i];
            } else if (i >= 54 && i <= 55) {
                Abin[i - 32] = bin[i];
            }
        }

        janus_utils_bin2dec(Abin, 24, A);

        printf("The key\n\n");
        for (i = 0; i < AES_KEYLEN; i++)
            printf("%02x ", key[i]);

        printf("\n\n");

        printf("\n\n");
        printf("The IV\n\n");
        for (i = 0; i < AES_KEYLEN; i++)
            printf("%02x ", IVwhole[i]);

        printf("\n\n");

        printf("\n\n");
        printf("The associated data A\n\n");
        for (i = 0; i < adlen; i++) {
            printf("%02x ", A[i]);
            if ((i + 1) % 16 == 0)
                printf("\n");
        }

        printf("\n\n");

        printf("\n\n");
        printf("The plaintext P\n\n");
        for (i = 0; i < msglen; i++) {
            printf("%02x ", P[i]);
            if ((i + 1) % 16 == 0)
                printf("\n");
        }

        printf("\n\n");

        printf("\n\n");

        for (i = 0; i < 100000; i++) {
            encStart = clock();
            AEGIS256_generation_encryption(key, IVwhole, A, adlen, P, C, msglen, Tprim);
            encEnd = clock();
            cpu_time_used = ((double) (encEnd - encStart)) / CLOCKS_PER_SEC;
            avgEnc += cpu_time_used;
        }

        avgEnc /= 100000;
        printf("\n\nDid gen-enc 100000 times, average seconds taken: %.10f \n", avgEnc);
        printf("The ciphertext\n\n");
        for (i = 0; i < msglen; i++)
            printf("%02x ", C[i]);
        printf("\n\n");
        printf("The authentication tag\n\n");
        for (i = 0; i < AES_BLOCKLEN; i++)
            printf("%02x ", Tprim[i]);
        printf("\n\n");

        // Decryption/verification
        for (i = 0; i < msglen; i++)
            P[i] = 0;
        for (i = 0; i < AES_BLOCKLEN; i++)
            Tprim[i] = 0;

        for (i = 0; i < 100000; i++) {
            encStart = clock();
            AEGIS256_decryption_verification(key, IVwhole, A, adlen, P, C, msglen, Tprim);
            encEnd = clock();
            cpu_time_used = ((double) (encEnd - encStart)) / CLOCKS_PER_SEC;
            avgDec += cpu_time_used;
        }

        avgDec /= 100000;
        printf("\n\nDid dec-ver 100000 times, average seconds taken: %.10f \n", avgDec);

        printf("The reconstructed plaintext\n\n");
        for (i = 0; i < msglen; i++)
            printf("%02x ", P[i]);
        printf("\n\n");
        printf("The reconstructed authentication tag\n\n");
        for (i = 0; i < AES_BLOCKLEN; i++)
            printf("%02x ", Tprim[i]);
        printf("\n\n");

        // --- packet 1 ---        
        janus_utils_dec2bin(IV, 4, IVbin);

        for (i = 0; i < 32; i++) {
            bin[i + 22] = IVbin[i];
        }

        janus_utils_bin2dec(bin, 64, aegispkt1bytes);
        janus_packet_set_bytes(aegispkt1, aegispkt1bytes);

        // Initialize state.
        state = janus_tx_state_new((params->verbose > 1));

        // Transmit.
        janus_simple_tx_execute(simple_tx, aegispkt1, state);

        janus_utils_dec2bin(aegispkt1bytes, JANUS_MIN_PKT_SIZE, bin);

        for (i = 0; i < 64; i++)
            fprintf(fp, "%u", bin[i]);
        fclose(fp);

        // --- packet 2 ---
        fp = fopen("/home/ttm4128/aegisnocargopkt2.txt", "wb");
        janus_utils_dec2bin(C, 12, cBin);

        aegispkt2 = janus_packet_new(params->verbose);
        janus_uint8_t* aegispkt2bytes = janus_packet_get_bytes(aegispkt2);
        janus_utils_dec2bin(aegispkt2bytes, JANUS_MIN_PKT_SIZE, bin);

        for (i = 0; i < 32; i++) {
            bin[i + 22] = cBin[i];
        }

        janus_utils_bin2dec(bin, 64, aegispkt2bytes);
        janus_packet_set_bytes(aegispkt2, aegispkt2bytes);

        // Initialize state.
        state = janus_tx_state_new((params->verbose > 1));

        // Transmit.
        janus_simple_tx_execute(simple_tx, aegispkt2, state);

        janus_utils_dec2bin(aegispkt2bytes, JANUS_MIN_PKT_SIZE, bin);

        for (i = 0; i < 64; i++)
            fprintf(fp, "%u", bin[i]);
        fclose(fp);

        // --- packet 3 ---
        fp = fopen("/home/ttm4128/aegisnocargopkt3.txt", "wb");
        aegispkt3 = janus_packet_new(params->verbose);
        janus_uint8_t* aegispkt3bytes = janus_packet_get_bytes(aegispkt3);
        janus_utils_dec2bin(aegispkt3bytes, JANUS_MIN_PKT_SIZE, bin);

        for (i = 0; i < 32; i++) {
            bin[i + 22] = cBin[i + 32];
        }

        janus_utils_bin2dec(bin, 64, aegispkt3bytes);
        janus_packet_set_bytes(aegispkt3, aegispkt3bytes);

        // Initialize state.
        state = janus_tx_state_new((params->verbose > 1));

        // Transmit.
        janus_simple_tx_execute(simple_tx, aegispkt3, state);

        janus_utils_dec2bin(aegispkt3bytes, JANUS_MIN_PKT_SIZE, bin);

        for (i = 0; i < 64; i++)
            fprintf(fp, "%u", bin[i]);
        fclose(fp);

        // --- packet 4 ---
        fp = fopen("/home/ttm4128/aegisnocargopkt4.txt", "wb");
        aegispkt4 = janus_packet_new(params->verbose);
        janus_uint8_t* aegispkt4bytes = janus_packet_get_bytes(aegispkt4);
        janus_utils_dec2bin(aegispkt4bytes, JANUS_MIN_PKT_SIZE, bin);

        for (i = 0; i < 32; i++) {
            bin[i + 22] = cBin[i + 64];
        }

        janus_utils_bin2dec(bin, 64, aegispkt4bytes);
        janus_packet_set_bytes(aegispkt4, aegispkt4bytes);

        // Initialize state.
        state = janus_tx_state_new((params->verbose > 1));

        // Transmit.
        janus_simple_tx_execute(simple_tx, aegispkt4, state);

        janus_utils_dec2bin(aegispkt4bytes, JANUS_MIN_PKT_SIZE, bin);

        for (i = 0; i < 64; i++)
            fprintf(fp, "%u", bin[i]);
        fclose(fp);

        // Dump.
        if (params->verbose > 0) {
            janus_tx_state_dump(state);
            //janus_packet_dump(packet);
//            janus_packet_dump(ccmpkt1);
//            janus_packet_dump(ccmpkt2);
//            janus_packet_dump(ccmpkt3);
//            janus_packet_dump(ccmpkt4);
//            janus_packet_dump(ccmpkt5);
            janus_packet_dump(aegispkt1);
            janus_packet_dump(aegispkt2);
            janus_packet_dump(aegispkt3);
            janus_packet_dump(aegispkt4);
        }

//        // Cleanup.
        janus_tx_state_free(state);
//        janus_packet_free(ccmpkt1);
//        janus_packet_free(ccmpkt2);
//        janus_packet_free(ccmpkt3);
//        janus_packet_free(ccmpkt4);
//        janus_packet_free(ccmpkt5);
        //janus_packet_free(packet);
        janus_packet_free(aegispkt1);
        janus_packet_free(aegispkt2);
        janus_packet_free(aegispkt3);
        janus_packet_free(aegispkt4);

        janus_simple_tx_free(simple_tx);
        free(C);
    }
#endif

#ifdef CLI_RX
    {
        janus_simple_rx_t simple_rx = 0;
        janus_packet_t packet = 0;
        janus_rx_state_t state = 0;
        janus_carrier_sensing_t carrier_sensing = 0;
        unsigned queried_detection_time = 0;
        janus_real_t time;

        // Initialize rx.
        simple_rx = janus_simple_rx_new(params);
        if (!simple_rx) {
            fprintf(stderr, "ERROR: failed to initialize receiver.\n");
            janus_parameters_free(params);
            cli_options_free(cli_options);
            return 1;
        }

        // Initialize carrier sensing;
        carrier_sensing = janus_carrier_sensing_new(janus_simple_rx_get_rx(simple_rx));

        // Initialize packet.
        packet = janus_packet_new(params->verbose);

        // Initialize state.
        state = janus_rx_state_new(params);
        if (!state) {
            fprintf(stderr, "ERROR: failed to initialize receiver.\n");
            janus_parameters_free(params);
            cli_options_free(cli_options);
            return 1;
        }

        // Receive.
        while (!done) {
            int rv = janus_simple_rx_execute(simple_rx, packet, state);

            if (rv < 0) {
                if (rv == JANUS_ERROR_OVERRUN) {
                    fprintf(stderr, "Error buffer-overrun\n");
                }
                break;
            }

            if (rv > 0) {
                // Dump.
                // Incorrect packet only if verbosity >= 3
                if (params->verbose >= 3 ||
                        (params->verbose > 0 &&
                        (janus_packet_get_validity(packet) &&
                        janus_packet_get_cargo_error(packet) == 0))) {
                    janus_rx_state_dump(state);
                    janus_packet_dump(packet);
                    janus_packet_reset(packet);
                }
                queried_detection_time = 0;
                janus_carrier_sensing_reset(carrier_sensing);
            } else {
                if (params->verbose > 0 && janus_simple_rx_has_detected(simple_rx) && !queried_detection_time) {
                    fprintf(stderr, "-> Triggering detection (%.6f)\n", janus_simple_rx_get_first_detection_time(simple_rx));
                    queried_detection_time = 1;
                }

                if (janus_carrier_sensing_execute(carrier_sensing, &time) > 0)
                    fprintf(stderr, "-> Busy channel t=%f w=%e b=%e\n", time,
                        janus_carrier_sensing_window_power(carrier_sensing),
                        janus_carrier_sensing_background_power(carrier_sensing));
            }
        }

        // Cleanup.
        janus_rx_state_free(state);
        janus_packet_free(packet);
        janus_carrier_sensing_free(carrier_sensing);
        janus_simple_rx_free(simple_rx);
    }
#endif

    janus_parameters_free(params);
    cli_options_free(cli_options);

    return 0;
}
