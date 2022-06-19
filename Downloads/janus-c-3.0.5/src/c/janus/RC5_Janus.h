#ifndef RC5_JANUS_H
#define RC5_JANUS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

static uint16_t ROTL(uint16_t x, uint16_t y, uint16_t w);
static uint16_t ROTR(uint16_t x, uint16_t y, uint16_t w);
static void RC5_encrypt(uint16_t* pt, uint16_t* ct, uint16_t* S, uint16_t w, uint16_t r);
static void RC5_decrypt(uint16_t* ct, uint16_t* pt, uint16_t* S, uint16_t w, uint16_t r);
static void RC5_initialize(uint8_t* K, uint16_t* S, uint16_t w, uint16_t r, uint16_t b);
uint32_t Janus_RC5_encrypt(uint32_t inblock, uint8_t* key);
uint32_t Janus_RC5_decrypt(uint32_t inblock, uint8_t* key);
#endif /* RC5_JANUS_H */

