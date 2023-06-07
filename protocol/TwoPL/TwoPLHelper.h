/*
 * @Author: lxc
 * @Date: 06/07/2023
 */

#pragma once

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
    static bool request_write_lock(ITable* table, const void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();

        if(meta & 0x1)
            return false;
        (*meta_atomic).fetch_or(0x1);
        return true;
    }

    static bool request_read_lock(ITable* table, const void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();

        if(meta & 0x2)
            return false;
        (*meta_atomic).fetch_or(0x2);
        return true;
    }
#endif

};