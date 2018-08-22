#ifndef _SLIPLIB_H
#define _SLIPLIB_H

/*!	\file sliplib.h
	\brief COSMOS SLIP support include file
	A library providing support functions for the COSMOS specific SLIP protocol.
	\defgroup sliplib COSMOS SLIP encoding library
*/

#include "configCosmos.h"

#include "cosmos-errno.h"
#include <cstring>

//! \ingroup sliplib
//! \defgroup sliplib_constants COSMOS SLIP support constants
//! @{

//! SLIP Buffer End character
#define SLIP_FEND 0xc0
//! SLIP Buffer Escape character
#define SLIP_FESC 0xdb
//! SLIP Buffer Escaped End character
#define SLIP_TFEND 0xdc
//! SLIP Buffer Escaped Escape character
#define SLIP_TFESC 0xdd

//! CRC-16-CCITT Normal
#define CRC16CCITT 0x1021
//! CRC-16-CCITT Reversed
#define CRC16CCITTR 0x8408
//! CRC-16-CCITT Reversed Reciprocal
#define CRC16CCITTRR 0x8810

//! @}

//! \ingroup sliplib
//! \defgroup sliplib_functions COSMOS SLIP support functions
//! @{

int32_t slip_check_crc(uint8_t *sbuf, uint16_t ssize);
int32_t slip_unpack(uint8_t *sbuf, uint16_t ssize, uint8_t *rbuf, uint16_t rsize);
int32_t slip_decode(uint8_t *sbuf, uint16_t ssize, uint8_t *rbuf, uint16_t rsize);
int32_t slip_pack(uint8_t *rbuf, uint16_t rsize, uint8_t *sbuf, uint16_t ssize);
int32_t slip_encode(uint8_t *rbuf, uint16_t rsize, uint8_t *sbuf, uint16_t ssize);
uint16_t slip_calc_crc(uint8_t *buf, uint16_t size);
uint16_t slip_get_crc(uint8_t *buf, uint16_t size);
uint16_t slip_set_crc(uint8_t *buf, uint16_t size);

//! @}

#endif
