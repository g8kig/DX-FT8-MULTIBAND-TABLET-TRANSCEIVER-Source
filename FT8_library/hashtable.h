#ifndef _INCLUDE_HASHTABLE_H_
#define _INCLUDE_HASHTABLE_H_

#include "message.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

void hashtable_init(int hashtable_max_size);
void hashtable_delete();
int hashtable_get_size();
void hashtable_cleanup(uint8_t max_age);
void hashtable_add(const char* callsign, uint32_t hash);
bool hashtable_lookup(ftx_callsign_hash_type_t hash_type, uint32_t hash, char* callsign);

extern ftx_callsign_hash_interface_t hash_if;

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_HASHTABLE_H_
