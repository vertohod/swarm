#ifndef DEFINES_KEY_STORE_H
#define DEFINES_KEY_STORE_H

#include "key_store.h"

#define UNIQUE_MODEL            swarm::key_store_set
#define MULTI_MODEL             swarm::key_store_mset

#define UNIQUE_SORTED_STORE     swarm::key_set_t
#define UNIQUE_UNSORTED_STORE   swarm::key_uset_t
#define MULTI_SORTED_STORE      swarm::key_mset_t
#define MULTI_UNSORTED_STORE    swarm::key_umset_t

#define DEFINITION_KEY_STORE(MODEL, STORE, KEY) \
    auto key_##KEY##_store_ptr = std::make_shared<MODEL<STORE, const KEY>>(); \
    keys_stores.insert(std::make_pair(key_##KEY##_store_ptr->name(), key_##KEY##_store_ptr)); \
    set_unique_keys_flag(MODEL<STORE, const KEY>::is_unique());
#endif
