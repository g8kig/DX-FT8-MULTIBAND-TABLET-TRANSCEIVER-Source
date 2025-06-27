/*
 * constants.h
 *
 *  Created on: Sep 17, 2019
 *      Author: user
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <stdint.h>
#include <stdbool.h>

extern const int ND;                  // Data symbols
extern const int NS;                  // Sync symbols (3 @ Costas 7x7)
extern const int NN;                  // Total channel symbols (79)
extern const int N;                   // Number of bits in the encoded message
extern const int K;                   // Number of payload bits
extern const int M;                   // Number of checksum bits
extern const int K_BYTES;             // Number of whole bytes needed to store K bits
extern const uint16_t CRC_POLYNOMIAL; // CRC-14 polynomial without the leading (MSB) 1
extern const int CRC_WIDTH;           // CRC width in bits

extern uint8_t tones[79];

// Costas 7x7 tone pattern
extern const uint8_t kCostas_map[7];

// Gray code map
extern const uint8_t kGray_map[8];

// Parity generator matrix for (174,91) LDPC code, stored in bitpacked format (MSB first)
// extern const uint8_t kGenerator[M][K_BYTES];
extern const uint8_t kGenerator[83][12];

// this is the LDPC(174,91) parity check matrix.
// 83 rows.
// each row describes one parity check.
// each number is an index into the codeword (1-origin).
// the codeword bits mentioned in each row must xor to zero.
// From WSJT-X's ldpc_174_91_c_reordered_parity.f90.
extern const uint8_t kNm[83][7];

// Mn from WSJT-X's bpdecode174.f90.
// each row corresponds to a codeword bit.
// the numbers indicate which three parity
// checks (rows in Nm) refer to the codeword bit.
// 1-origin.

extern const uint8_t kMn[174][3];

// Number of rows (columns in C/C++) in the array Nm.
extern const uint8_t kNrw[83];

#endif /* CONSTANTS_H_ */
