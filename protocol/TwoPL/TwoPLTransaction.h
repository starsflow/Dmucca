/*
 * @Author: lxc
 * @Date: 05/22/2023
 */
#pragma once

#include "Counter.h"
#include "Database.h"
#include "Global.h"
#include "SafeQueue.h"
#include "Table.h"
#include "TwoPLHelper.h"

struct RWItem {
    void* key;
    // storage the read results
    void* value;
    uint32_t table_id;

    RWItem(void* k, void* v, uint32_t id) : key(k), value(v), table_id(id) {}
};

class TwoPLTransaction {
public:
    std::vector<RWItem> read_set, write_set;
    uint32_t txn_id;
    TransactionResult status;
    SafeQuene<TwoPLTransaction>* _ready_execute_queue;
    Database* _db;

public:
    TwoPLTransaction(Database* db, SafeQuene<TwoPLTransaction>* queue) : _ready_execute_queue(queue), _db(db) {}

    TwoPLTransaction(std::vector<RWItem> rs, std::vector<RWItem> ws, uint32_t tid, TransactionResult sts)
        : read_set(rs), write_set(ws), txn_id(tid), status(sts) {}

    ~TwoPLTransaction() {
        read_set.clear();
        write_set.clear();
    }

    ITable* get_table(int table_id) { return static_cast<ITable*>(_db->find_table(table_id)); }

    void set_txn_id() {
        txn_id = Counter::generate_unique_id();
        LOG(INFO) << "the generated id is " << txn_id;
    }

    template <class KeyType>
    RWItem* get_read_write_item(std::vector<RWItem>& set, const KeyType* key) {
        for (std::size_t i = 0; i < set.size(); i++) {
            if (*static_cast<KeyType*>(set[i].key) == *key) return &set[i];
        }
        return nullptr;
    }

    template <class KeyType, class ValueType>
    void append_read_set(std::uint32_t table_id, KeyType* key, ValueType* value) {
        RWItem* read_item = get_read_write_item<KeyType>(read_set, key);
        if (!read_item) {
            read_set.emplace_back(RWItem(key, nullptr, table_id));
        }
    }

    template <class KeyType, class ValueType>
    void append_write_set(std::uint32_t table_id, KeyType* key, ValueType* value) {
        RWItem* read_item = get_read_write_item<KeyType>(read_set, key);
        RWItem* write_item = get_read_write_item<KeyType>(write_set, key);
        if (!write_item) {
            write_set.emplace_back(RWItem(key, value, table_id));
        } else {
            write_item->value = value;
        }

        if (read_item) {
            read_item->value = value;
        } else {
            read_set.emplace_back(RWItem(key, value, table_id));
        }
    }

    void insert_queue() { _ready_execute_queue->push(TwoPLTransaction(read_set, write_set, txn_id, status)); }

    // template<class KeyType, class ValueType>
    // bool search_for_read(std::uint32_t table_id, KeyType* key, ValueType* value) {
    //     ITable* table = get_table(table_id);
    //     RWItem* read_item = get_read_write_item(read_set, key);
    //     RWItem* write_item = get_read_write_item(write_set, key);

    //     //abort if the record is write locked by other txn
    //     bool can_read_locked = TwoPLHelper::set_read_lock_bit(table, key);
    //     if(!can_read_locked && !write_item) {
    //         return false;
    //     }

    //     if(!read_item) {
    //         read_item = new RWItem(key, nullptr, table_id);
    //         read_set.emplace_back(*read_item);
    //     }
    //     return true;
    // }

    // template<class KeyType, class ValueType>
    // bool search_for_write(std::uint32_t table_id, KeyType* key, ValueType* value) {
    //     ITable* table = get_table(table_id);
    //     RWItem* read_item = get_read_write_item(read_set, key);
    //     RWItem* write_item = get_read_write_item(write_set, key);

    //     bool can_write_lock = TwoPLHelper::set_write_lock_bit(table, key);
    //     if(!can_write_lock && !(read_item || write_item)) {
    //         return false;
    //     }

    //     //whether the record to be updated is in txn's read set
    //     if(read_item) {
    //         read_item->value = value;
    //     }

    //     if(!write_item) {
    //         write_item = new RWItem(key, value, table_id);
    //         write_set.emplace_back(*write_item);
    //     } else {
    //         write_item->value = value;
    //     }
    //     return true;
    // }

    // void release_read_write_set_lock(std::uint32_t table_id) {
    //     ITable* table = get_table(table_id);
    //     for(auto read_item : read_set) {
    //         auto* key = read_item.key;
    //         TwoPLHelper::reset_read_lock_bit(table, key);
    //     }

    //     for(auto write_item : write_set) {
    //         auto* key = write_item.key;
    //         TwoPLHelper::reset_write_lock_bit(table, key);
    //     }
    // }
};
