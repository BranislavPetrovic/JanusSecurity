/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.h to edit this template
 */

/* 
 * File:   ccm.h
 * Author: slobodanp
 *
 * Created on February 25, 2022, 10:50 AM
 */

#ifndef CCM_H
#define CCM_H

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#define MAX_T 16
#define MAX_N 13
#define FIFTEEN 15
#define ALIMIT_1 65280
#define ALIMIT_2 4294967296

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>
typedef unsigned long ulong;

uint8_t CCM_gen_encrypt(uint8_t* key, uint8_t* N, uint8_t n, uint8_t* A, ulong a, uint8_t* P, ulong p, uint8_t t, uint8_t* C, ulong c);
uint8_t CCM_decrypt_verify(uint8_t* key, uint8_t* C, ulong c, uint8_t t, uint8_t* N, uint8_t n, uint8_t* A, ulong a, uint8_t* P, ulong p);
void cbcformat(uint8_t* N, uint8_t n, uint8_t* A, ulong a, uint8_t* P, ulong p, uint8_t t, uint8_t* B, ulong Blen);
void initctrblock(uint8_t* N, uint8_t n, uint8_t* Ctr0);
void genSstream(uint8_t* key, uint8_t* Ctr0, uint8_t q, ulong m, uint8_t* S_stream, ulong s, uint8_t* S0);
void cbcmac(uint8_t* key, uint8_t *B, ulong b, uint8_t* T, uint8_t t);
static uint8_t ismember(uint8_t* a, ulong n, ulong e);
static void genS(uint8_t *key, uint8_t* ctr, uint8_t* S);
static void genctrvals(uint8_t** ctrarr, uint8_t q, ulong m);
static void tobinary(ulong a, uint8_t* v, ulong n);
void tobytes(uint8_t* v, ulong n, uint8_t* b);

#endif /* CCM_H */

