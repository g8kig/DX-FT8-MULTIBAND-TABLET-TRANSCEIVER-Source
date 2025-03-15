#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

static struct
{
    char callsign[12]; ///> Up to 11 symbols of callsign + trailing zeros (always filled)
    uint32_t hash;     ///> 8 MSBs contain the age of callsign; 22 LSBs contain hash value
}* callsign_hashtable;

static int callsign_hashtable_size;
static int callsign_hashtable_max_size;

ftx_callsign_hash_interface_t hash_if = {
    .lookup_hash = hashtable_lookup,
    .save_hash = hashtable_add
};

void hashtable_init(int hashtable_max_size)
{
    callsign_hashtable_size = 0;
    callsign_hashtable_max_size = hashtable_max_size;
    callsign_hashtable = malloc(sizeof(*callsign_hashtable) * callsign_hashtable_max_size);
    memset(callsign_hashtable, 0, sizeof(callsign_hashtable) * callsign_hashtable_max_size);
}

void hashtable_delete()
{
    callsign_hashtable_max_size = 0;
    free(callsign_hashtable);
}

int hashtable_get_size()
{
    return callsign_hashtable_size;
}

void hashtable_cleanup(uint8_t max_age)
{
    for (int idx_hash = 0; idx_hash < callsign_hashtable_max_size; ++idx_hash)
    {
        if (callsign_hashtable[idx_hash].callsign[0] != 0)
        {
            uint8_t age = (uint8_t)(callsign_hashtable[idx_hash].hash >> 24);
            if (age > max_age)
            {
                // free the hash entry
                callsign_hashtable[idx_hash].callsign[0] = 0;
                callsign_hashtable[idx_hash].hash = 0;
                callsign_hashtable_size--;
            }
            else
            {
                // increase callsign age
                callsign_hashtable[idx_hash].hash = (((uint32_t)age + 1u) << 24) | (callsign_hashtable[idx_hash].hash & 0x3FFFFFu);
            }
        }
    }
}

void hashtable_add(const char* callsign, uint32_t hash)
{
    uint16_t hash10 = (hash >> 12) & 0x3FFu;
    int idx_hash = (hash10 * 23) % callsign_hashtable_max_size;
    while (callsign_hashtable[idx_hash].callsign[0] != 0)
    {
        if (((callsign_hashtable[idx_hash].hash & 0x3FFFFFu) == hash) && (0 == strcmp(callsign_hashtable[idx_hash].callsign, callsign)))
        {
            // reset age
            callsign_hashtable[idx_hash].hash &= 0x3FFFFFu;
            return;
        }
        else
        {
            // Move on to check the next entry in hash table
            idx_hash = (idx_hash + 1) % callsign_hashtable_max_size;
        }
    }
    callsign_hashtable_size++;
    strncpy(callsign_hashtable[idx_hash].callsign, callsign, 11);
    callsign_hashtable[idx_hash].callsign[11] = 0;
    callsign_hashtable[idx_hash].hash = hash;
}

bool hashtable_lookup(ftx_callsign_hash_type_t hash_type, uint32_t hash, char* callsign)
{
    uint8_t hash_shift = (hash_type == FTX_CALLSIGN_HASH_10_BITS) ? 12 : (hash_type == FTX_CALLSIGN_HASH_12_BITS ? 10 : 0);
    uint16_t hash10 = (hash >> (12 - hash_shift)) & 0x3FFu;
    int idx_hash = (hash10 * 23) % callsign_hashtable_max_size;
    while (callsign_hashtable[idx_hash].callsign[0] != 0)
    {
        if (((callsign_hashtable[idx_hash].hash & 0x3FFFFFu) >> hash_shift) == hash)
        {
            strcpy(callsign, callsign_hashtable[idx_hash].callsign);
            return true;
        }
        // Move on to check the next entry in hash table
        idx_hash = (idx_hash + 1) % callsign_hashtable_max_size;
    }
    callsign[0] = 0;
    return false;
}
