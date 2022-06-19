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
// Authors: Ricardo Martins, Luigi Elia D'Amaro, Roberto Petroccia        *
//*************************************************************************

// ISO C headers.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// JANUS headers.
#include <janus/defaults.h>
#include <janus/dump.h>
#include <janus/crc.h>
#include <janus/packet.h>
#include <janus/error.h>
#include <janus/class_user_id.def>
#include <janus/intervals.def>
#include <janus/codec/codecs.h>
#include <janus/utils/memory.h>
#include <janus/utils/dec2bin.h>
#include <janus/utils/imath.h>
#include <janus/aes.h>
#include <janus/ccm.h>
#include <janus/aegis_128.h>

#define BMASK(var, bit)  ((var >> (8 - bit)) & 0x01U)
#define LMASK(var, bits) (var & (0xFFU >> (8 - bits)))
#define HMASK(var, bits) (var & (0xFFU << (8 - bits)))
#define CBC 1
#define CTR 1
#define ECB 1

static struct janus_codecs codecs;
static janus_uint8_t codecs_initialized = 0;

void janus_packet_dump(const janus_packet_t);
void ccm(uint8_t*, uint8_t*, uint8_t*);
void aegis(uint8_t*, uint8_t*, uint64_t, uint64_t);
void toBinary(uint8_t);
void baselineToFile(char*);

void
reset()
{
  janus_codecs_reset(&codecs);
}

//! Packet reservation time or repeat interval flag values.
enum janus_packet_reservation_repeat_flag
{
  //! Reservation time.
  JANUS_PACKET_RESERVATION_TIME = 0,
  //! Repeat interval.
  JANUS_PACKET_REPEAT_INTERVAL = 1
};

//! Janus packet.
//! Tha packet has the following fields:
//! Version Number (4 bits: 0-3)
//! Mobility Flag (1 bit: 4)
//! Schedule Flag (1 bit: 5)
//! Tx/Rx Flag (1 bit: 6)
//! Forwarding Capability Flag (1 bit: 7)
//! Class User Identifier (8 bits: 8-15)
//! Application Type (6 bits: 16-21)
//! Application Data (34 bits: 22-55)
//!   If Schedule Flag is 'On' (set=1), first 8 bits of Application Data has the following fields:
//!     Reservation Time (bit set=0) or Repeat Interval (bit set=1) Flag (1 bit: 22)
//!     Reservation Time or Repeat Interval lookup table index (7 bit: 23-29)
//! CRC (8 bits: 56-63)
//! Optional Cargo (0-JANUS_MAX_PKT_CARGO_SIZE bits)
struct janus_packet
{
  //! Packet bytes.
  janus_uint8_t bytes[JANUS_MIN_PKT_SIZE];
  //! Optional cargo.
  janus_uint8_t cargo[JANUS_MAX_PKT_CARGO_SIZE];
  //! Optional desired cargo size.
  janus_uint16_t desired_cargo_size;
  //! Cargo size
  unsigned cargo_size;
  //! Class user ID
  janus_uint8_t class_user_id;
  //! Application Type
  janus_uint8_t application_type;
  //! Number of bits of application data
  unsigned application_data_size;
  //! Application fields pointer
  janus_app_fields_t app_fields;
  //! Validity pass (0: not valid, 1: basic packet valid, 2: cargo valid)
  janus_uint8_t validity;
  //! Cargo decoding error state (0: no error)
  int cargo_error;
};

//! Number of elements in the c_reservation_time_table and c_repeat_interval_table tables.
static const unsigned c_interval_count = sizeof(c_repeat_interval_table) / sizeof(double);

static enum janus_packet_interval_lookup_result
interval_lookup_value(const double dvalue, double* evalue1, double* evalue2, int* index, unsigned reservation_repeat)
{
  const double* table = (reservation_repeat == JANUS_PACKET_RESERVATION_TIME) ? c_reservation_time_table : c_repeat_interval_table;
  
  unsigned i = 0;

  *evalue1 = 0.0;
  *evalue2 = 0.0;
  *index = -1;

  if (dvalue < table[0])
  {
    *evalue1 = table[0];
    return JANUS_PACKET_ERROR_MIN;
  }

  if (dvalue > table[c_interval_count - 1])
  {
    *evalue1 = table[c_interval_count - 1];
    return JANUS_PACKET_ERROR_MAX;
  }

  for (i = 0; i < c_interval_count; ++i)
  {
    if (dvalue == table[i])
    {
      *evalue1 = table[i];
      *index = i;
      return JANUS_PACKET_EXACT_TIME;
    }

    if (dvalue < table[i])
    {
      // Accepting if in 5% tollerance of the interval step.
      double interval_tollerance = ((table[i] - table[i - 1]) * 0.05);

      if ((table[i] - dvalue) < interval_tollerance)
      {
        *evalue1 = table[i];
        *index = i;
        return JANUS_PACKET_APPROXIMATED_TIME;
      }

      if ((dvalue - table[i - 1]) < interval_tollerance)
      {
        *evalue1 = table[i - 1];
        *index = i - 1;
        return JANUS_PACKET_APPROXIMATED_TIME;
      }

      *evalue1 = table[i - 1];
      *evalue2 = table[i];
      *index = i - 1;
      return JANUS_PACKET_BETWEEN_TWO_VALUES;
    }
  }

  *evalue1 = table[c_interval_count - 1];
  return JANUS_PACKET_ERROR_MAX;
}

janus_packet_t
janus_packet_new(janus_uint8_t verbose)
{
  janus_packet_t pkt = JANUS_UTILS_MEMORY_NEW_ZERO(struct janus_packet, 1);
  janus_packet_set_version(pkt, JANUS_VERSION);

  pkt->app_fields = janus_app_fields_new();
  
  if (codecs_initialized == 0)
  {
    janus_codecs_reset(&codecs);
    codecs_initialized = 1;
    atexit(reset);
  }
  janus_codecs_set_verbose(&codecs, verbose);

  return pkt;
}

void
janus_packet_reset(janus_packet_t pkt)
{
  janus_app_fields_t fieldsp = pkt->app_fields;
  janus_app_fields_reset(fieldsp);
  
  memset(pkt->bytes, 0, sizeof(struct janus_packet));

  janus_packet_set_version(pkt, JANUS_VERSION);
  pkt->app_fields = fieldsp;
}

void
janus_packet_free(janus_packet_t pkt)
{
  janus_app_fields_free(pkt->app_fields);
  JANUS_UTILS_MEMORY_FREE(pkt);
}

const janus_uint8_t*
janus_packet_get_bytes(const janus_packet_t pkt)
{
  return pkt->bytes;
}

void
janus_packet_set_bytes(janus_packet_t pkt, const janus_uint8_t* bytes)
{
  memcpy(pkt->bytes, bytes, JANUS_MIN_PKT_SIZE);
}

void
janus_packet_base_decode(janus_packet_t pkt)
{
  janus_packet_decode_class_id(pkt);
  janus_packet_decode_app_type(pkt);
  janus_packet_set_validity(pkt, 1);
  
  pkt->application_data_size = (janus_packet_get_schedule(pkt)) ? 26 : 34;

}

void
janus_packet_set_version(janus_packet_t pkt, janus_uint8_t version)
{
  pkt->bytes[0] = LMASK(pkt->bytes[0], 4) | (LMASK(version, 4) << 4);
}

janus_uint8_t
janus_packet_get_version(const janus_packet_t pkt)
{
  return HMASK(pkt->bytes[0], 4) >> 4;
}

void
janus_packet_set_mobility(janus_packet_t pkt, janus_uint8_t mobility)
{
  pkt->bytes[0] = HMASK(pkt->bytes[0], 4) | ((mobility ? 0x01U : 0x00U) << 3) | LMASK(pkt->bytes[0], 3);
}

janus_uint8_t
janus_packet_get_mobility(const janus_packet_t pkt)
{
  return BMASK(pkt->bytes[0], 5);
}

static void
janus_packet_set_schedule(janus_packet_t pkt, janus_uint8_t schedule)
{
  pkt->bytes[0] = HMASK(pkt->bytes[0], 5) | ((schedule ? 0x01U : 0x00U) << 2) | LMASK(pkt->bytes[0], 2);
}

janus_uint8_t
janus_packet_get_schedule(const janus_packet_t pkt)
{
  return BMASK(pkt->bytes[0], 6);
}

void
janus_packet_set_tx_rx(janus_packet_t pkt, janus_uint8_t tx_rx)
{
  pkt->bytes[0] = HMASK(pkt->bytes[0], 6) | ((tx_rx ? 0x01U : 0x00U) << 1) | LMASK(pkt->bytes[0], 1);
}

janus_uint8_t
janus_packet_get_tx_rx(const janus_packet_t pkt)
{
  return BMASK(pkt->bytes[0], 7);
}

void
janus_packet_set_forward(janus_packet_t pkt, janus_uint8_t forward)
{
  pkt->bytes[0] = HMASK(pkt->bytes[0], 7) | (forward ? 0x01U : 0x00U);
}

janus_uint8_t
janus_packet_get_forward(const janus_packet_t pkt)
{
  return BMASK(pkt->bytes[0], 8);
}

void
janus_packet_set_class_id(janus_packet_t pkt, janus_uint8_t class_id)
{
  pkt->bytes[1] = class_id;
  pkt->class_user_id = class_id;
}

janus_uint8_t
janus_packet_get_class_id(const janus_packet_t pkt)
{
  return pkt->class_user_id;
}

janus_uint8_t
janus_packet_decode_class_id(janus_packet_t pkt)
{
  pkt->class_user_id = pkt->bytes[1];

  return pkt->class_user_id;
}

void
janus_packet_set_app_type(janus_packet_t pkt, janus_uint8_t app_type)
{
  pkt->bytes[2] = LMASK(pkt->bytes[2], 2) | (LMASK(app_type, 6) << 2);
  pkt->application_type = app_type;
}

janus_uint8_t
janus_packet_get_app_type(const janus_packet_t pkt)
{
  return pkt->application_type;
}

janus_uint8_t
janus_packet_decode_app_type(const janus_packet_t pkt)
{
  pkt->application_type = HMASK(pkt->bytes[2], 6) >> 2;

  return pkt->application_type;
}

static void
janus_packet_set_reservation_repeat_flag(janus_packet_t pkt, unsigned reservation_repeat)
{
  reservation_repeat = (reservation_repeat == JANUS_PACKET_REPEAT_INTERVAL) ? JANUS_PACKET_REPEAT_INTERVAL : JANUS_PACKET_RESERVATION_TIME;
  pkt->bytes[2] = HMASK(pkt->bytes[2], 6) | (reservation_repeat << 1) | LMASK(pkt->bytes[2], 1);
}

static janus_uint8_t
janus_packet_get_reservation_repeat_flag(const janus_packet_t pkt)
{
  return BMASK(pkt->bytes[2], 7);
}

void
janus_packet_unset_tx_interval(janus_packet_t pkt)
{
  janus_packet_set_schedule(pkt, 0);
  janus_packet_set_reservation_repeat_flag(pkt, 0);
  pkt->bytes[2] = HMASK(pkt->bytes[2], 7);
  pkt->bytes[3] = LMASK(pkt->bytes[3], 2);
}

static enum janus_packet_interval_lookup_result
janus_packet_set_tx_interval(janus_packet_t pkt, const double dvalue, double* evalue1, double* evalue2, unsigned reservation_repeat)
{
  int index;
  enum janus_packet_interval_lookup_result rv;

  rv = interval_lookup_value(dvalue, evalue1, evalue2, &index, reservation_repeat);

  if (rv == JANUS_PACKET_EXACT_TIME || rv == JANUS_PACKET_APPROXIMATED_TIME)
  {
    janus_packet_set_schedule(pkt, 1);
    janus_packet_set_reservation_repeat_flag(pkt, reservation_repeat);
    pkt->bytes[2] = HMASK(pkt->bytes[2], 7) | (BMASK(index, 2));
    pkt->bytes[3] = LMASK(pkt->bytes[3], 2) | (LMASK(index, 6) << 2);
  }

  return rv;
}

enum janus_packet_interval_lookup_result
janus_packet_set_tx_reservation_time(janus_packet_t pkt, const double dvalue, double* evalue1, double* evalue2)
{
  return janus_packet_set_tx_interval(pkt, dvalue, evalue1, evalue2, JANUS_PACKET_RESERVATION_TIME);
}

enum janus_packet_interval_lookup_result
janus_packet_set_tx_repeat_interval(janus_packet_t pkt, double dvalue, double* evalue1, double* evalue2)
{
  return janus_packet_set_tx_interval(pkt, dvalue, evalue1, evalue2, JANUS_PACKET_REPEAT_INTERVAL);
}

double
janus_packet_get_tx_interval(const janus_packet_t pkt, janus_uint8_t* reservation_repeat_flag)
{
  if (janus_packet_get_schedule(pkt))
  {
    unsigned idx = (BMASK(pkt->bytes[2], 8) << 6) | (HMASK(pkt->bytes[3], 6) >> 2);
    *reservation_repeat_flag = janus_packet_get_reservation_repeat_flag(pkt);
    return (*reservation_repeat_flag == JANUS_PACKET_RESERVATION_TIME) ? c_reservation_time_table[idx] : c_repeat_interval_table[idx];
  }
  else
  {
    *reservation_repeat_flag = 2;
    return 0.0;
  }
}

janus_int64_t
janus_packet_decode_application_data(janus_packet_t pkt)
{
  janus_uint8_t app_data_size;
  janus_int64_t app_data = janus_packet_get_application_data(pkt, &app_data_size);
  
  int decode_error = janus_codecs_decode_app_data(&codecs, pkt->class_user_id, pkt->application_type, app_data, app_data_size, &pkt->cargo_size, pkt->app_fields);
  if (decode_error != 0)
    return -1;
  
  return app_data;
}

janus_uint8_t
janus_packet_encode_application_data(const janus_packet_t pkt)
{
  janus_uint64_t app_data = 0;
  
  int rv = janus_codecs_encode_app_data(&codecs, pkt->class_user_id, pkt->application_type, pkt->desired_cargo_size, pkt->app_fields, pkt->application_data_size, &pkt->cargo_size, &app_data);

  if (rv == 0)
    janus_packet_set_application_data(pkt, app_data);
  
  return rv;
}

janus_uint8_t
janus_packet_set_application_data(const janus_packet_t pkt, janus_uint64_t app_data)
{
  janus_uint8_t schedule = janus_packet_get_schedule(pkt);
  pkt->application_data_size = (schedule) ? 26 : 34;
  
  if (app_data >= (0x1ULL << pkt->application_data_size))
  {
    return 1;
  }

  if (schedule)
  {
    pkt->bytes[3] = HMASK(pkt->bytes[3], 6) | (janus_uint8_t)(app_data >> 24);
    pkt->bytes[4] = (janus_uint8_t)(app_data >> 16);
    pkt->bytes[5] = (janus_uint8_t)(app_data >> 8);
    pkt->bytes[6] = (janus_uint8_t)app_data;
  }
  else
  {
    pkt->bytes[2] = HMASK(pkt->bytes[2], 6) | (janus_uint8_t)(app_data >> 32);
    pkt->bytes[3] = (janus_uint8_t)(app_data >> 24);
    pkt->bytes[4] = (janus_uint8_t)(app_data >> 16);
    pkt->bytes[5] = (janus_uint8_t)(app_data >> 8);
    pkt->bytes[6] = (janus_uint8_t)app_data;
  }

  return 0;
}

janus_uint64_t
janus_packet_get_application_data(const janus_packet_t pkt, janus_uint8_t* app_data_size)
{
  janus_uint64_t rv;

  *app_data_size = pkt->application_data_size;
  if (pkt->application_data_size == 26)
  {
    rv = ((janus_uint64_t)LMASK(pkt->bytes[3], 2) << 24);
  }
  else
  {
    rv = ((janus_uint64_t)LMASK(pkt->bytes[2], 2) << 32) | ((janus_uint64_t)pkt->bytes[3] << 24);
  }
  rv |= ((janus_uint64_t)pkt->bytes[4] << 16) | ((janus_uint64_t)pkt->bytes[5] << 8) | pkt->bytes[6];

  return rv;
}

unsigned
janus_packet_get_application_data_size(const janus_packet_t pkt)
{
  return pkt->application_data_size;
}

int
janus_packet_set_application_data_fields(janus_packet_t pkt, janus_app_fields_t app_fields)
{
  unsigned i;
  
  for (i = 0; i < app_fields->field_count; ++i)
  {
    janus_app_fields_add_field(pkt->app_fields, app_fields->fields[i].name, app_fields->fields[i].value);
  }
  
  return 0;
}

int
janus_packet_get_application_data_fields(const janus_packet_t pkt, janus_app_fields_t app_fields)
{
  unsigned i;
  
  for (i = 0; i < pkt->app_fields->field_count; ++i)
  {
    janus_app_fields_add_field(app_fields, pkt->app_fields->fields[i].name, pkt->app_fields->fields[i].value);
  }
  
  return 0;
}

void
janus_packet_set_cargo_size(const janus_packet_t pkt, unsigned cargo_size)
{
  pkt->cargo_size = cargo_size;
}

unsigned
janus_packet_get_cargo_size(const janus_packet_t pkt)
{
  return pkt->cargo_size;
}

unsigned
janus_packet_get_desired_cargo_size(const janus_packet_t pkt)
{
  return pkt->desired_cargo_size;
}

int
janus_packet_set_cargo(janus_packet_t pkt, janus_uint8_t* cargo, unsigned cargo_size)
{
  if (cargo_size > JANUS_MAX_PKT_CARGO_SIZE)
    return JANUS_ERROR_CARGO_SIZE;

  memcpy(pkt->cargo, cargo, cargo_size);
  pkt->desired_cargo_size = cargo_size;

  return JANUS_ERROR_NONE;
}

janus_uint8_t*
janus_packet_get_cargo(const janus_packet_t pkt)
{
  return pkt->cargo;
}

int
janus_packet_encode_cargo(janus_packet_t pkt)
{
  int rv = 0;
  
  janus_uint8_t** cargo = (janus_uint8_t**)calloc(1, sizeof(janus_uint8_t*));
  *cargo = JANUS_UTILS_MEMORY_NEW_ZERO(janus_uint8_t, 1);
  unsigned cargo_size = 0;

  rv = janus_codecs_encode_cargo(&codecs, pkt->class_user_id, pkt->application_type, pkt->app_fields, cargo, &cargo_size);

#if 0
  fprintf(stderr, "CARGO_SIZE = %d\n", cargo_size);
#endif
  
  if (rv == 0)
  {
    memcpy(pkt->cargo, *cargo, cargo_size);
    memset(pkt->cargo + cargo_size, 0, JANUS_MAX_PKT_CARGO_SIZE - cargo_size);
    JANUS_UTILS_MEMORY_FREE(*cargo);

    pkt->desired_cargo_size = cargo_size;
  }
  // else
  // {
  //   fprintf(stderr, "ERROR: failed to encode cargo using plugin class id %d app type %d (error code %d)\n", class_user_id, app_type, rv);
  // }
  JANUS_UTILS_MEMORY_FREE(cargo);
  
  return rv;
}

int
janus_packet_decode_cargo(const janus_packet_t pkt)
{
  int rv = 0;
  
  janus_uint8_t* cargo = pkt->cargo;
  rv = janus_codecs_decode_cargo(&codecs, pkt->class_user_id, pkt->application_type, cargo, pkt->cargo_size, &(pkt->app_fields));
  

#if 0
  for (int i = 0; i < pkt->app_fields->field_count; ++i) {
    fprintf(stderr, "janus_packet_decode_cargo name %s value %s\n", pkt->app_fields->fields[i].name, pkt->app_fields->fields[i].value);
  }
#endif
  
  // if (rv != 0)
  // {
  //   fprintf(stderr, "ERROR: failed to decode cargo using plugin class id %d app type %d (error code %d)\n", class_user_id, app_type, rv);
  // }

  return rv;
}

janus_uint8_t
janus_packet_set_crc(janus_packet_t pkt)
{
  janus_uint8_t crc = janus_crc(pkt->bytes, JANUS_MIN_PKT_SIZE - 1, 0);
  pkt->bytes[JANUS_MIN_PKT_SIZE - 1] = crc;
  return crc;
}

janus_uint8_t
janus_packet_get_crc(const janus_packet_t pkt)
{
  return pkt->bytes[JANUS_MIN_PKT_SIZE - 1];
}

janus_uint8_t
janus_packet_get_crc_validity(const janus_packet_t pkt)
{
  janus_uint8_t pcrc = janus_packet_get_crc(pkt);
  janus_uint8_t ccrc = janus_crc(pkt->bytes, JANUS_MIN_PKT_SIZE - 1, 0);

  return (pcrc == ccrc);
}

void
janus_packet_set_validity(janus_packet_t pkt, janus_uint8_t val)
{
  pkt->validity = val;
}

janus_uint8_t
janus_packet_get_validity(const janus_packet_t pkt)
{
  return pkt->validity;
}

void
janus_packet_set_cargo_error(janus_packet_t pkt, int val)
{
  pkt->cargo_error = val;
}

int
janus_packet_get_cargo_error(const janus_packet_t pkt)
{
  return pkt->cargo_error;
}

void remove_spaces(char* s) {
    char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

//char* htoi (const char *ptr, unsigned cargo_size)
//{
////unsigned int value = 0;
///*char* bin = ;*/
//    remove_spaces(ptr);
//    char ch = *ptr;
//    int i = -1;
//    char bin[10000] = "";
//    char onebin[] = "0000";
//
//    while (i < cargo_size)
//    {
//       i++;
//       ch = *(ptr + i);
//
//       switch(ch)
//       {
//            case '0':
//                strcat(bin, onebin);
//                break;
//            case '1':
//                *(bin + i) = "0001";
//                break;
//            case '2':
//                *(bin + i) = "0010";
//                break;
//            case '3':
//                *(bin + i) = "0011";
//                break;
//            case '4':
//               *(bin + i) = "0100";
//               break;
//            case '5':
//               *(bin + i) = "0101";
//               break;
//            case '6':
//               *(bin + i) = "0110";
//               break;
//            case '7':
//               *(bin + i) = "0111";
//               break;
//            case '8':
//               *(bin + i) = "1000";
//               break;
//            case '9':
//                *(bin + i) = "1001";
//                break;
//            case 'A':
//                *(bin + i) = "1010";
//                break;
//            case 'B':
//               *(bin + i) = "1011";
//               break;
//            case 'C':
//               *(bin + i) = "1100";
//               break;
//            case 'D':
//               *(bin + i) = "1101";
//               break;
//            case 'E':
//               *(bin + i) = "1110";
//               break;
//            case 'F':
//               *(bin + i) = "1111";
//               break;
//       }
//    }
//
//    return bin;
//}

void
janus_packet_dump(const janus_packet_t pkt) {
    unsigned i, j;
    janus_uint8_t bits[8];
    const janus_uint8_t* bytes = janus_packet_get_bytes(pkt);
    unsigned cargo_size = 0;
    char string_bytes_decimal[JANUS_MIN_PKT_SIZE * 4 + 2];
    char string_bytes_hexadecimal[JANUS_MIN_PKT_SIZE * 4 + 2];
    janus_uint8_t bin[JANUS_MIN_PKT_SIZE * 8];
    char string_fields_binary[JANUS_MIN_PKT_SIZE * 8 + 21];
    char string_fields_binary_nopipe[JANUS_MIN_PKT_SIZE * 8 + 21];
    char string_class[256];
    char string_app_data[35];
    uint8_t ass_data[32];

    FILE* fp;
    fp = fopen("/tmp/januspktbin.txt", "a");

    sprintf(string_bytes_decimal, "|");
    for (i = 0; i < JANUS_MIN_PKT_SIZE; ++i)
        APPEND_FORMATTED(string_bytes_decimal, "%3u|", bytes[i]);

    sprintf(string_bytes_hexadecimal, "|");
    for (i = 0; i < JANUS_MIN_PKT_SIZE; ++i) {
        APPEND_FORMATTED(string_bytes_hexadecimal, " %02X|", bytes[i]);
    }

    janus_utils_dec2bin(pkt->bytes, JANUS_MIN_PKT_SIZE, bin);
    sprintf(string_fields_binary, "|");
    for (i = 0; i < JANUS_MIN_PKT_SIZE * 8; i++) {
        if ((i > 3 && i < 9) || i == 16 || i == 22 || (janus_packet_get_schedule(pkt) && (i == 23 || i == 30)) || i == 56)
            APPEND_FORMATTED(string_fields_binary, "|%s", "");
        APPEND_FORMATTED(string_fields_binary, "%u", bin[i]);
    }
    APPEND_FORMATTED(string_fields_binary, "|%s", "");

    for (i = 0; i < JANUS_MIN_PKT_SIZE * 8; i++) {
        APPEND_FORMATTED(string_fields_binary_nopipe, "%u", bin[i]);
    }

    sprintf(string_class, "%d (%s)", janus_packet_get_class_id(pkt), class_user_id[janus_packet_get_class_id(pkt)]);

    {
        janus_uint8_t app_data_size;
        janus_uint64_t app_data = janus_packet_get_application_data(pkt, &app_data_size);
        unsigned string_application_bytes = app_data_size / 8;
        janus_uint8_t* adp = ((janus_uint8_t*) & app_data) + string_application_bytes;
        janus_utils_dec2bin_byte(*adp--, bits);
        sprintf(string_app_data, "%u%u", bits[6], bits[7]);
        for (i = 0; i < string_application_bytes; ++i) {
            janus_utils_dec2bin_byte(*adp--, bits);
            for (j = 0; j < 8; ++j)
                APPEND_FORMATTED(string_app_data, "%u", bits[j]);
        }
    }

    //printf(string_fields_binary_nopipe);
    //fprintf(fp, string_fields_binary_nopipe);
    //fflush(stdout);
    //baselineToFile(string_fields_binary_nopipe);

    JANUS_DUMP("Packet", "Bytes (decimal)", "%s", string_bytes_decimal);
    JANUS_DUMP("Packet", "Bytes (hex)", "%s", string_bytes_hexadecimal);
    JANUS_DUMP("Packet", "Fields (binary)", "%s", string_fields_binary);
    JANUS_DUMP("Packet", "  Version Number (4 bits)", "%u", janus_packet_get_version(pkt));
    JANUS_DUMP("Packet", "  Mobility Flag (1 bit)", "%u", janus_packet_get_mobility(pkt));
    JANUS_DUMP("Packet", "  Schedule Flag (1 bit)", "%u", janus_packet_get_schedule(pkt));
    JANUS_DUMP("Packet", "  Tx/Rx Flag (1 bit)", "%u", janus_packet_get_tx_rx(pkt));
    JANUS_DUMP("Packet", "  Forwarding Capability (1 bit)", "%u", janus_packet_get_forward(pkt));
    JANUS_DUMP("Packet", "  Class User Identifier (8 bits)", "%s", string_class);
    JANUS_DUMP("Packet", "  Application Type (6 bits)", "%u", janus_packet_get_app_type(pkt));
    JANUS_DUMP("Packet", (janus_packet_get_application_data_size(pkt) == 26)
            ? "  Application Data (26 bits)"
            : "  Application Data (34 bits)", "%s", string_app_data);

    if (janus_packet_get_schedule(pkt)) {
        janus_uint8_t reservation_repeat_flag;
        double tx_interval = janus_packet_get_tx_interval(pkt, &reservation_repeat_flag);
        double evalue1, evalue2;
        int index;
        char string_reservation_repeat[44];
        JANUS_DUMP("Packet", "    Reservation/Repeat Flag (1 bit)", "%u", reservation_repeat_flag);
        if (reservation_repeat_flag == JANUS_PACKET_RESERVATION_TIME) {
            interval_lookup_value(tx_interval, &evalue1, &evalue2, &index, reservation_repeat_flag);
            sprintf(string_reservation_repeat, "%.8f (lookup table index %u)", tx_interval, index);
            JANUS_DUMP("Packet", "    Reservation Time (7 bits)", "%s", string_reservation_repeat);
        } else {
            interval_lookup_value(tx_interval, &evalue1, &evalue2, &index, reservation_repeat_flag);
            sprintf(string_reservation_repeat, "%.8f (lookup table index %u)", tx_interval, index);
            JANUS_DUMP("Packet", "    Repeat Interval (7 bits)", "%s", string_reservation_repeat);
        }
    }

    if (janus_packet_get_validity(pkt) > 0) {
        janus_app_fields_t app_fields = janus_app_fields_new();
        janus_packet_get_application_data_fields(pkt, app_fields);
        cargo_size = janus_packet_get_cargo_size(pkt);

        JANUS_DUMP("Packet", "  Cargo Size", "%u", cargo_size);

        JANUS_DUMP("Packet", "  CRC (8 bits)", "%u", janus_packet_get_crc(pkt));
        JANUS_DUMP("Packet", "CRC Validity", "%u", (janus_packet_get_validity(pkt) > 0));

        const janus_uint8_t* cargo = janus_packet_get_cargo(pkt);

        if (app_fields->field_count > 0) {
            JANUS_DUMP("Packet", "Application Data Fields", "%s", "");
            for (i = 0; i < app_fields->field_count; ++i) {
                char name[64];
                sprintf(name, "  %s", app_fields->fields[i].name);
                JANUS_DUMP("Packet", name, "%s", app_fields->fields[i].value);
            }
        }

        if (cargo_size > 0) {
            if (janus_packet_get_validity(pkt) == 2) {
                char string_cargo[JANUS_MAX_PKT_CARGO_SIZE * 3 + 1];
                string_cargo[0] = '\0';
                for (i = 0; i < cargo_size; ++i) {
                    APPEND_FORMATTED(string_cargo, "%02X ", cargo[i]);
                    //APPEND_FORMATTED(string_cargo, "%3u ", cargo[i]);
                }

                remove_spaces(string_cargo);

                char ch = *string_cargo;
                char cargo_bin[8 * cargo_size];
                cargo_bin[0] = '\0';
                //char onebin[] = "0000";

                for (int i = 0; i < strlen(string_cargo); i++) {
                    ch = *(string_cargo + i);
                    switch (ch) {
                        case '0':
                            strcat(cargo_bin, "0000");
                            break;
                        case '1':
                            strcat(cargo_bin, "0001");
                            break;
                        case '2':
                            strcat(cargo_bin, "0010");
                            break;
                        case '3':
                            strcat(cargo_bin, "0011");
                            break;
                        case '4':
                            strcat(cargo_bin, "0100");
                            break;
                        case '5':
                            strcat(cargo_bin, "0101");
                            break;
                        case '6':
                            strcat(cargo_bin, "0110");
                            break;
                        case '7':
                            strcat(cargo_bin, "0111");
                            break;
                        case '8':
                            strcat(cargo_bin, "1000");
                            break;
                        case '9':
                            strcat(cargo_bin, "1001");
                            break;
                        case 'A':
                            strcat(cargo_bin, "1010");
                            break;
                        case 'B':
                            strcat(cargo_bin, "1011");
                            break;
                        case 'C':
                            strcat(cargo_bin, "1100");
                            break;
                        case 'D':
                            strcat(cargo_bin, "1101");
                            break;
                        case 'E':
                            strcat(cargo_bin, "1110");
                            break;
                        case 'F':
                            strcat(cargo_bin, "1111");
                            break;
                    }
                }

                JANUS_DUMP("Packet", "Cargo (hex)", "%s", string_cargo);
                JANUS_DUMP("Packet", "Cargo (bin)", "%s", cargo_bin);
                fprintf(fp, cargo_bin);
                //fprintf(fp, htoi(string_cargo, cargo_size));
                fclose(fp);

                string_cargo[0] = '\0';
                sprintf(string_cargo, "\"");
                for (i = 0; i < cargo_size; ++i)
                    APPEND_FORMATTED(string_cargo, "%c", cargo[i]);
                APPEND_FORMATTED(string_cargo, "\"%s", "");
                JANUS_DUMP("Packet", "Cargo (ASCII)", "%s", string_cargo);
            } else {
                JANUS_DUMP("Packet", "Cargo Error", "%d", janus_packet_get_cargo_error(pkt));
            }
        }

        janus_app_fields_free(app_fields);
    } else {
        JANUS_DUMP("Packet", "  CRC (8 bits)", "%u", janus_packet_get_crc(pkt));
        JANUS_DUMP("Packet", "CRC Validity", "%u", (janus_packet_get_validity(pkt) > 0));
        // fprintf(stderr, "Error CRC\n");
    }

    uint8_t header[21];
    uint8_t crc[8];
    uint8_t payload[40];

    for (int i = 0; i < 21; i++) {
        header[i] = (uint8_t) string_fields_binary_nopipe[i];
    }

    for (int i = 0; i < 8; i++) {
        crc[i] = (uint8_t) string_fields_binary_nopipe[56 + i];
    }

    for (int i = 0; i < 22; i++) {
        ass_data[i] = header[i] & 1;
    }

    for (int i = 0; i < 8; i++) {
        ass_data[22 + i] = crc[i] & 1;
    }

    for (int i = 30; i < 32; i++)
        ass_data[i] = 0;

    for (int i = 0; i < 34; i++) {
        payload[i] = (uint8_t) string_fields_binary_nopipe[22 + i] & 1;
    }

    uint8_t bass_data[4];
    uint8_t byte_payload[5];
    uint8_t N[] = {0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00};

    tobytes(ass_data, 32, bass_data);
    tobytes(payload, 40, byte_payload);
    
    /*
      header[22] = "\0";
      crc[8] = "\0";
     */

    //JANUS_DUMP("Packet", "assdata", "%s", ass_data);
    //JANUS_DUMP("Packet", "payload", "%s", payload);

//    for (int i = 0; i < 300; i++) {
//        ccm(N, bass_data, byte_payload);
//    }

    //aegis(bass_data, byte_payload, 4, 5);
    //ccm(N, bass_data, byte_payload);
}

//void ccm(uint8_t* N, uint8_t* A, uint8_t* P)
//{
//    int i;
//
//#if defined(AES256)
//    printf("\nTesting AES256\n\n");
//#elif defined(AES192)
//    printf("\nTesting AES192\n\n");
//#elif defined(AES128)
//    printf("\nTesting CCM generation/encryption and decryption/verification with AES\n\n");
//#else
//    printf("You need to specify a symbol between AES128, AES192 or AES256. Exiting");
//    return 0;
//#endif
//
//    uint8_t key[] = {0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf};      
//    uint8_t n = 7;
//    ulong a = 4;
//    ulong p = 5;
//    uint8_t t = 4;
//    int Tlen = 1;
//    ulong maxInv = pow(2, 1);
//    double risk = pow(2, -20);
//    FILE* fp;
//    fp = fopen("/home/ttm4128/maclengthvsrisk_2e1.csv", "a");
//    uint8_t err;
//    ulong c = p + t;
//    uint8_t* C = malloc(c * sizeof (uint8_t));
//    char cBin[8 * c];
//    cBin[0] = "\0";
//    //uint8_t curC = *C;
//    
//    while(risk <= 1)
//    {
//        Tlen = log2(maxInv / risk);
//        fprintf(fp, "%d, %.50f", Tlen, risk);
//        //maxInv /= 2;
//        risk *= 2;
//        fprintf(fp, "\n");
//    }
//    
//    printf("The key\n\n");
//    for (i = 0; i < AES_KEYLEN; i++)
//        printf("%02x ", key[i]);
//    
//    printf("\n\n");
//    
//    /*
//    for (i = 0; i < AES_KEYLEN; i++)
//    {
//        toBinary(key[i]);
//    }
//     */
//
//    printf("\n\n");
//    printf("The nonce N\n\n");
//    for (i = 0; i < n; i++)
//        printf("%02x ", N[i]);
//    
//    printf("\n\n");
//    
//    for (i = 0; i < n; i++)
//    {
//        toBinary(N[i]);
//    }
//
//    printf("\n\n");
//    printf("The associated data A\n\n");
//    for (i = 0; i < a; i++) {
//        printf("%02x ", A[i]);
//        if ((i + 1) % 16 == 0)
//            printf("\n");
//    }
//    
//    printf("\n\n");
//    
//    for (i = 0; i < a; i++)
//    {
//        toBinary(A[i]);
//    }
//
//    printf("\n\n");
//    printf("The plaintext P\n\n");
//    for (i = 0; i < p; i++) {
//        printf("%02x ", P[i]);
//        if ((i + 1) % 16 == 0)
//            printf("\n");
//    }
//    
//    printf("\n\n");
//    
//    /*
//    for (i = 0; i < p; i++)
//    {
//        toBinary(P[i]);
//    }
//     */
//    
//    printf("\n\n");
//    printf("The length of the MAC tag T is t = %d\n\n", t);
//    
//    err = CCM_gen_encrypt(key, N, n, A, a, P, p, t, C, c);
//    
//    if (err == 0) {
//        printf("The authenticated and encrypted output C\n\n");
//        for (i = 0; i < p + t; i++) {
//            printf("%02x ", C[i]);
//            
//            if ((i + 1) % AES_BLOCKLEN == 0)
//                printf("\n");
//        }        
//        printf("\n\n");
//        
//        for(i = 0; i < c; i++)
//        {
//            toBinary(C[i]);
//        }
//    } else {
//        printf("Error in input data, error code = %d", err);
//    }
//    
//    printf("\n\n");
//    
//    free(C);
//}

//void aegis(uint8_t* AD, uint8_t* P, uint64_t ADlen, uint64_t MSGlen)
//{
//    int i;
//    uint8_t key[] = {0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//    uint8_t IV[] = {0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//    uint8_t Tprim[AES_BLOCKLEN];
//    uint8_t* C = malloc(MSGlen * sizeof (uint8_t));
//    
//    printf("The key\n\n");
//    for (i = 0; i < AES_KEYLEN; i++)
//        printf("%02x ", key[i]);
//    
//    printf("\n\n");
//    
//    printf("The initialization vector IV\n\n");
//    for (i = 0; i < AES_KEYLEN; i++)
//        printf("%02x ", IV[i]);
//    
//    printf("\n\n");
//    
//    printf("The associated data\n\n");
//    for (i = 0; i < ADlen; i++)
//        printf("%02x ", AD[i]);
//    
//    printf("\n\n");
//    
//    printf("The plaintext\n\n");
//    for (i = 0; i < MSGlen; i++)
//        printf("%02x ", P[i]);
//    
//    printf("\n\n");
//    
//    // Generation/encryption
//    AEGIS_generation_encryption(key, IV, AD, ADlen, P, C, MSGlen, Tprim);
//    
//    printf("The ciphertext\n\n");
//    for (i = 0; i < MSGlen; i++)
//        printf("%02x ", C[i]);
//    
//    printf("\n\n");
//    
//    printf("The authentication tag\n\n");
//    for (i = 0; i < AES_BLOCKLEN; i++)
//        printf("%02x ", Tprim[i]);
//    
//    printf("\n\n");
//    
//    // Decryption/verification
//    for (i = 0; i < MSGlen; i++)
//        P[i] = 0;
//    
//    for (i = 0; i < AES_BLOCKLEN; i++)
//        Tprim[i] = 0;
//    
//    AEGIS_decryption_verification(key, IV, AD, ADlen, P, C, MSGlen, Tprim);
//    
//    printf("The reconstructed plaintext\n\n");
//    for (i = 0; i < MSGlen; i++)
//        printf("%02x ", P[i]);
//    
//    printf("\n\n");
//    
//    printf("The reconstructed authentication tag\n\n");
//    for (i = 0; i < AES_BLOCKLEN; i++)
//        printf("%02x ", Tprim[i]);
//    
//    printf("\n\n");
//    
//    free(C);
//}
//
//void toBinary(uint8_t a) {
//    uint8_t i;
//    FILE* fp;
//    fp = fopen("/home/ttm4128/januspktbinccm.txt", "a");
//
//    for (i = 0x80; i != 0; i >>= 1) {
//        printf("%c", (a & i) ? '1' : '0');
//        fprintf(fp, "%c", (a & i) ? '1' : '0');
//    }
//
//    fclose(fp);
//}
//
//void baselineToFile(char* pkt)
//{
//    FILE* fp;
//    fp = fopen("/home/ttm4128/januspktbinccm.txt", "wb+");
//    
//    fprintf(fp, pkt);
//    fclose(fp);
//}
