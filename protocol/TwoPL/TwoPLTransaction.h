/*
 * @Author: lxc
 * @Date: 05/22/2023
 */
#pragma once

#include <sys/types.h>
#include "Database.h"
#include "Global.h"
#include "Operation.h"
#include "Table.h"
#include "TwoPL/TwoPLHelper.h"

struct RWItem {
    void* key;
    //storage the read results
    void* value;
};

class TwoPLTransaction {
public:
    std::vector<RWItem> read_set;
    std::vector<RWItem> write_set;
    int32_t txn_id;
    TransactionResult res;
    Database* _db;


public:
    TwoPLTransaction(Database* db) : _db(db) {}

    ~TwoPLTransaction() {
        read_set.clear();
        write_set.clear();
    }

    ITable* get_table(int table_id) {
        return static_cast<ITable *>(_db->find_table(table_id));
    }

    RWItem* get_read_write_item(std::vector<RWItem> &set, const void* key) {
        for(int i = 0; i < set.size(); i++){
            if(set[i].key == key)
                return &set[i]; 
        }
        return nullptr;
    }

    template<class KeyType, class ValueType>
    bool search_for_read(std::size_t table_id, KeyType* key, ValueType* value) {
        ITable* table = get_table(table_id);
        RWItem* read_item = get_read_write_item(read_set, key);
        RWItem* write_item = get_read_write_item(write_set, key);

        //abort if the record is write locked by other txn
        bool can_read_locked = TwoPLHelper::set_read_lock_bit(table, key);
        if(!can_read_locked && !write_item) {
            return false;
        }

        if(!read_item) {
            read_item = new RWItem(key, nullptr);
            read_set.emplace_back(*read_item);
        } 
        return true;
    }

    template<class KeyType, class ValueType>
    bool search_for_write(std::size_t table_id, KeyType* key, ValueType* value) {
        ITable* table = get_table(table_id);
        RWItem* read_item = get_read_write_item(read_set, key);
        RWItem* write_item = get_read_write_item(write_set, key);

        bool can_write_lock = TwoPLHelper::set_write_lock_bit(table, key);
        if(!can_write_lock && !(read_item || write_item)) {
            return false;
        }
        
        //whether the record to be updated is in txn's read set
        if(read_item) {
            read_item->value = value;
        }

        if(!write_item) {
            write_item = new RWItem(key, value);
            write_set.emplace_back(*write_item);
        } else {
            write_item->value = value;
        }
        return true;
    }

    void release_read_write_set_lock(std::size_t table_id) {
        ITable* table = get_table(table_id);
        for(auto read_item : read_set) {
            auto* key = read_item.key;
            TwoPLHelper::reset_read_lock_bit(table, key);
        }

        for(auto write_item : write_set) {
            auto* key = write_item.key;
            TwoPLHelper::reset_write_lock_bit(table, key);
        }
    }
};
