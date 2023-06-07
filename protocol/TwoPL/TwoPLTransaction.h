/*
 * @Author: lxc
 * @Date: 05/22/2023
 */
#pragma once

#include <cstddef>
#include "Database.h"
#include "Global.h"
#include "Operation.h"
#include "TwoPL/TwoPLHelper.h"
#include "TwoPLRWSet.h"

class TwoPLTransaction {
public:
    std::vector<ReadItem> read_set;
    std::vector<WriteItem> write_set;
    int32_t txn_id;
    TransactionResult res;
    Database* _db;

public:
    TwoPLTransaction(Database* db) : _db(db) {}

    ITable* get_table(int table_id) {
        return static_cast<ITable *>(_db->find_table(table_id));
    }

    template<class KeyType, class ValueType>
    bool search_for_read(std::size_t table_id, KeyType* key, ValueType* value) {
        ReadItem read_item(key, value);
        ITable* table = get_table(table_id);
        bool is_read_locked = TwoPLHelper::request_read_lock(table, key);
        if(is_read_locked)
            return false;
        read_set.push_back(read_item);
        return true;
    }

    template<class KeyType, class ValueType>
    bool search_for_write(std::size_t table_id, KeyType* key, ValueType* value) {
        
    }
};
