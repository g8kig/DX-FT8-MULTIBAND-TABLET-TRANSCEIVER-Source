#ifndef _INCLUDE_ENCODE_H_
#define _INCLUDE_ENCODE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Generate FT8 tone sequence from payload data
/// @param[in] payload - 10 byte array consisting of 77 bit payload
/// @param[out] tones  - array of FT8_NN (79) bytes to store the generated tones (encoded as 0..7)
void ft8_encode(const uint8_t* payload, uint8_t* tones);

/// Generate FT4 tone sequence from payload data
/// @param[in] payload - 10 byte array consisting of 77 bit payload
/// @param[out] tones  - array of FT4_NN (105) bytes to store the generated tones (encoded as 0..3)
void ft4_encode(const uint8_t* payload, uint8_t* tones);

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_ENCODE_H_
