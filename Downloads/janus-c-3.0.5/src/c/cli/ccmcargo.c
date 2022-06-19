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

        // --- CCM, cargo ---
        janus_packet_t pkt = 0;
        pkt = janus_packet_new(params->verbose);
        janus_uint8_t* pktbytes = janus_packet_get_bytes(pkt);
        FILE* fp;
        janus_uint8_t bin[JANUS_MIN_PKT_SIZE * 8];
        clock_t encStart, encEnd;
        double cpu_time_used;
        double avgEnc, avgDec = 0;
        int i;
        janus_utils_dec2bin(pktbytes, JANUS_MIN_PKT_SIZE, bin);
        fp = fopen("/home/ttm4128/ccmcargopkt.txt", "wb");
        uint8_t pktadbbin[34];
        uint8_t N[] = {0x00, 0x00, 0x00, 0x03};
        uint8_t Ndouble[] = {0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03};
        uint8_t Nbin[32];
        uint8_t key[] = {0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf};
        uint8_t F[] = {0x02};
        uint8_t Fbin[8];
        uint8_t A[3];
        uint8_t Abin[24];
        uint8_t n = 8;
        ulong a = 3;
        ulong p = 12;
        uint8_t t = 4;
        uint8_t err;
        ulong c = p + t;
        uint8_t* C = malloc(c * sizeof (uint8_t));
        uint8_t cBin[8 * c];
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
        printf("The nonce N\n\n");
        for (i = 0; i < n; i++)
            printf("%02x ", Ndouble[i]);

        printf("\n\n");

        printf("\n\n");
        printf("The associated data A\n\n");
        for (i = 0; i < a; i++) {
            printf("%02x ", A[i]);
            if ((i + 1) % 16 == 0)
                printf("\n");
        }

        printf("\n\n");

        printf("\n\n");
        printf("The plaintext P\n\n");
        for (i = 0; i < p; i++) {
            printf("%02x ", P[i]);
            if ((i + 1) % 16 == 0)
                printf("\n");
        }

        printf("\n\n");

        printf("\n\n");
        printf("The length of the MAC tag T is t = %d\n\n", t);

        err = CCM_gen_encrypt(key, Ndouble, n, A, a, P, p, t, C, c);

        for (i = 0; i < 100000; i++) {
            encStart = clock();
            err = CCM_gen_encrypt(key, Ndouble, n, A, a, P, p, t, C, c);
            encEnd = clock();
            cpu_time_used = ((double) (encEnd - encStart)) / CLOCKS_PER_SEC;
            avgEnc += cpu_time_used;
        }

        avgEnc /= 100000;
        printf("\n\nDid gen-enc 100000 times, average seconds taken: %.10f \n", avgEnc);

        for (i = 0; i < 100000; i++) {
            encStart = clock();
            err = CCM_decrypt_verify(key, C, c, t, Ndouble, n, A, a, P, p);
            encEnd = clock();
            cpu_time_used = ((double) (encEnd - encStart)) / CLOCKS_PER_SEC;
            avgDec += cpu_time_used;
        }

        avgDec /= 100000;
        printf("\n\nDid dec-ver 100000 times, average seconds taken: %.10f \n", avgDec);

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

        printf("\n\n");    
        
        // --- packet ---        
        janus_utils_dec2bin(N, 4, Nbin);

        for (i = 0; i < 26; i++) {
            bin[i + 30] = Nbin[i];
        }

        janus_utils_bin2dec(bin, 64, pktbytes);
        janus_packet_set_bytes(pkt, pktbytes);

        // Initialize state.
        state = janus_tx_state_new((params->verbose > 1));

        // Transmit.
        janus_simple_tx_execute(simple_tx, pkt, state);
        
        janus_utils_dec2bin(pktbytes, JANUS_MIN_PKT_SIZE, bin);
        
        for (i = 0; i < 64; i++) {
            fprintf(fp, "%u", bin[i]);
        }
        
        for (i = 0; i < 6; i++) {
            fprintf(fp, "%u", Nbin[i + 26]);
        }
        
        janus_utils_dec2bin(F, 1, Fbin);
        
        fprintf(fp, "%u", Fbin[6]);
        fprintf(fp, "%u", Fbin[7]);
        
        janus_utils_dec2bin(C, 16, cBin);
        
        for (i = 0; i < 124; i++) {
            fprintf(fp, "%u", cBin[i]);
        }
        
        fclose(fp);

        // Dump.
        if (params->verbose > 0) {
            janus_tx_state_dump(state);
            janus_packet_dump(pkt);
        }

        // Cleanup.
        janus_tx_state_free(state);
        janus_packet_free(pkt);
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
