/*
 * @Author: lxc
 * @Date: 06/07/2023
 */

#pragma once

#include "Table.h"

class TwoPLHelper {
public:
    using MetaType = std::atomic<uint64_t>;

public:
/*
    |   txn-id  |   read-lock-number    |  ...  | read-request-lock | write-request-lock | read-lock | write-lock |
    |  63...32  |     31...24           |  ...  |   3               |   2                |  1        |  0         |
*/
#if CC_FLAG == WAIT_DIE
    bool request_read_lock(ITable* table, void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();

        if (!(meta & 0x5)) return false;

        (*meta_atomic).store(meta | 0x8);
        return true;
    }

    bool request_write_lock(ITable* table, void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();

        if (!(meta & 0xf)) return false;

        (*meta_atomic).store(meta | 0x4);
        return true;
    }
#endif

#if CC_FLAG == NO_WAIT
    static bool set_write_lock_bits(ITable* table, void* key, uint32_t txn_id) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta, new_meta;
        do {
            meta = (*meta_atomic).load();
            LOG(INFO) << "before set write lock, txn id : " << txn_id << "; key : " << *static_cast<YKey*>(key)
                      << "; meta : " << (*meta_atomic).load();
            // is read or write locked ?
            if (meta & 0x3) return false;
            new_meta = ((((uint64_t)txn_id) << 32) | (meta & 0xffffffff)) | 0x1;
        } while (!meta_atomic->compare_exchange_strong(meta, new_meta));
        (*meta_atomic).store(new_meta);
        LOG(INFO) << "after set write lock, txn id : " << txn_id << "; key : " << *static_cast<YKey*>(key)
                  << "; meta : " << (*meta_atomic).load();
        return true;
    }

    static bool set_read_lock_bits(ITable* table, void* key, uint32_t txn_id) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta, new_meta;
        do {
            meta = (*meta_atomic).load();
            LOG(INFO) << "before set read lock, txn id : " << txn_id << "; key : " << *static_cast<YKey*>(key)
                      << "; meta : " << (*meta_atomic).load();
            // is write locked ?
            if (meta & 0x1) return false;
            new_meta = (meta + 0x1000000) | 0x2;
        } while (!meta_atomic->compare_exchange_strong(meta, new_meta));
        (*meta_atomic).store(new_meta);
        LOG(INFO) << "after set read lock, txn id : " << txn_id << "; key : " << *static_cast<YKey*>(key)
                  << "; meta : " << (*meta_atomic).load();
        return true;
    }

    static void release_read_lock_bits(ITable* table, void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta, new_meta;
        do {
            meta = (*meta_atomic).load();
            LOG(INFO) << "before release read lock, key : " << *static_cast<YKey*>(key) << "; meta : " << (*meta_atomic).load();
            uint64_t read_lock_cnt = (meta << 32) >> 56;
        if (read_lock_cnt > 1) {
            new_meta = meta - 0x1000000;
        } else if (read_lock_cnt) {
            new_meta = meta & 0xffffffff00fffffd;
        }
        }while (!meta_atomic->compare_exchange_strong(meta, new_meta));
        (*meta_atomic).store(new_meta);
        LOG(INFO) << "after release read lock, key : " << *static_cast<YKey*>(key) << "; meta : " << (*meta_atomic).load();
    }

    static void release_write_lock_bits(ITable* table, void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        LOG(INFO) << "before release write lock, key : " << *static_cast<YKey*>(key) << "; meta : " << (*meta_atomic).load();
        (*meta_atomic).fetch_and(0xfffffffe);
        LOG(INFO) << "before release write lock, key : " << *static_cast<YKey*>(key) << "; meta : " << (*meta_atomic).load();
    }

    // static void reset_read_lock_bit(ITable* table, void* key) {
    //     MetaType* meta_atomic = table->search_metadata(key);
    //     uint64_t meta = (*meta_atomic).load();
    //     meta = ~(~meta | 0x2);
    //     meta &= 0xffffffff;
    //     (*meta_atomic).store(meta);
    // }

    // static void reset_write_lock_bit(ITable* table, void* key) {
    //     MetaType* meta_atomic = table->search_metadata(key);
    //     uint64_t meta = (*meta_atomic).load();
    //     meta = ~(~meta | 0x3);
    //     meta &= 0xffffffff;
    //     (*meta_atomic).store(meta);
    // }

    static void reset_read_lock_bit(ITable* table, void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        (*meta_atomic).fetch_and(0xffffffff00fffffd);
    }

    static uint8_t get_read_lock_number_bits(ITable* table, void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();
        return (uint8_t)(meta >> 24);
    }

    static uint32_t get_txn_id_bits(ITable* table, void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();
        return (uint32_t)(meta >> 32);
    }

    // for refactoring
private:
    template <class Func>
    void _apply_with_modified_meta(Func func, ITable* table, void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();
        (*meta_atomic).store(Func(meta));
    }

    template <class Func>
    auto _apply_with_unmodified_meta(Func func, ITable* table, void* key) {
        MetaType* meta_atomic = table->search_metadata(key);
        uint64_t meta = (*meta_atomic).load();
        auto modified_meta = Func(meta);
        return modified_meta;
    }
#endif
};