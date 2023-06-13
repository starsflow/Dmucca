/*
 * @Author: lxc
 * @Date: 06/07/2023
 */

#pragma once

#include <cstdint>
#include "Table.h"
#include "TwoPL.h"

class TwoPLHelper {
public:
    using MetaType = std::atomic<uint32_t>;
public:
/*
    | read-request-lock | write-request-lock | read-lock | write-lock |
    |   3               |   2                |  1        |  0         |
*/  
#if CC_FLAG == WAIT_DIE
    bool request_read_lock(ITable* table, const void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();
        
        if(!(meta & 0x5))
            return false;
        
        (*meta_atomic).store(meta | 0x8);
        return true;
    }

    bool request_write_lock(ITable* table, const void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();
        
        if(!(meta & 0xf))
            return false;
        
        (*meta_atomic).store(meta | 0x4);
        return true;
    }
#endif

#if CC_FLAG == NO_WAIT
    static bool set_write_lock_bit(ITable* table, const void* key, uint32_t txn_id) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();

        if(meta & 0x3)
            return false;
        meta = (((uint64_t)txn_id) << 32) | (meta & 0xffffffff);
        (*meta_atomic).fetch_or(0x1);
        return true;
    }

    static bool set_read_lock_bit(ITable* table, const void* key, uint32_t txn_id) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();

        if(meta & 0x1)
            return false;
        meta = (((uint64_t)txn_id) << 32) | (meta & 0xffffffff);
        (*meta_atomic).fetch_or(0x2);
        return true;
    }

    static void reset_read_lock_bit(ITable* table, const void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();
        meta = ~(~meta | 0x2);  
        (*meta_atomic).store(meta);
    }

    static void reset_write_lock_bit(ITable* table, const void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();
        meta = ~(~meta | 0x3);
        (*meta_atomic).store(meta);
    }

    static uint32_t get_meta_upper32_bits(ITable* table, const void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();
        return (uint32_t)(meta >> 32);
    }
#endif

};