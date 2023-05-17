//
// Created by lxc on 5/9/23.
//

#pragma once

#include "Global.h"
#include "Database.h"
#include "ClassOf.h"
#include "Row.h"

//N: number of records in table
template<class KeyType, class ValueType>
class Table {
private:
    std::unordered_map<KeyType, ValueType, decltype(&KeyType::hash_key)> _table;
//    std::unordered_map<KeyType, ValueType>::hasher hasher;
    Database* _db;
    std::size_t _table_id;

public:
    Table(Database* db, std::size_t table_id) : _db(db), _table_id(table_id){}

    Table() {}

    virtual ~Table() = default;

    void *search(KeyType key) {
        _db->apply_with_locked_table(
            [&key](Table<KeyType, ValueType>* table){
                auto result = table->_table.find(key);
                if(result == table->_table.end())
                    return nullptr;
                else
                    return result->second;
            },
            _table_id
        );
    }

    void insert(KeyType key, ValueType value){
        _db->sum([](){}, 0);
        _db->apply_with_locked_table<std::function<void* (Table<KeyType, ValueType>*)>, KeyType, ValueType>(
            [key, value](Table<KeyType, ValueType>* table) {
                    table->_table[key] = value;
                    return nullptr;
                },
            _table_id
        );
    }

    void erase(KeyType key) {
        _db->apply_with_locked_table(
            [key](Table<KeyType, ValueType>* table) {
                    table->_table.erase(key);
                },
            _table_id
        );
    }



    auto bucket_number(KeyType key) { return hasher(key);}

    void garbage_collect(const void *key) {}

    std::size_t get_key_size() { return sizeof(KeyType); }

    std::size_t get_value_size() { return sizeof(ValueType); }

    std::size_t get_field_size() { return ClassOf<ValueType>::size(); }

    std::size_t get_table_id() { return _table_id; }

    Database* get_database() { return _db; }
};
