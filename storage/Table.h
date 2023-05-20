//
// Created by lxc on 5/9/23.
//

#pragma once

#include <atomic>
#include "ClassOf.h"
#include "Database.h"
#include "Global.h"
#include "Row.h"

class Database;
// N: number of records in table
template <class KeyType, class ValueType>
class Table {
public:
    using MetaType = std::atomic<uint64_t>;

    Table(Database *db, std::size_t table_id) : _db(db), _table_id(table_id) {}

    Table() {}

    virtual ~Table() = default;

    void *search_value(KeyType key) {
        return _db->apply_ref_with_locked_table<
            std::function<void *(Table<KeyType, ValueType> *)>, KeyType,
            ValueType>(
            [key](Table<KeyType, ValueType> *table) -> void * {
                auto result = table->_table.find(key);
                if (result == table->_table.end()) return nullptr;
                return &(std::get<0>(result->second));
            },
            _table_id);
    }

    void *search_metadata(KeyType key) {
        return _db->apply_ref_with_locked_table<
            std::function<void *(Table<KeyType, ValueType> *)>, KeyType,
            ValueType>(
            [key](Table<KeyType, ValueType> *table) -> void * {
                auto result = table->_table.find(key);
                if (result == table->_table.end()) return nullptr;
                return &(std::get<1>(result->second));
            },
            _table_id);
    }

    void insert(KeyType key, MetaType meta, ValueType value) {
        _db->apply_with_locked_table<
            std::function<void *(Table<KeyType, ValueType> *)>, KeyType,
            ValueType>(
            [key, &meta, value](Table<KeyType, ValueType> *table) {
                std::atomic_store(&std::get<0>(table->_table[key]), meta);
                std::get<1>(table->_table[key]) = value;
                return nullptr;
            },
            _table_id);
    }

    void erase(KeyType key) {
        _db->apply_with_locked_table<
            std::function<void *(Table<KeyType, ValueType> *)>, KeyType,
            ValueType>(
            [key](Table<KeyType, ValueType> *table) {
                table->_table.erase(key);
                return nullptr;
            },
            _table_id);
    }

    void print_table() {
        std::string title_string;
        std::stringstream titles_stream(KeyType::class_to_string().str() + ";" +
                                        ValueType::class_to_string().str());
        while (getline(titles_stream, title_string, ';')) {
            std::cout << title_string << "\t\t";
        }
        std::cout << "\n";

        for (std::pair<KeyType, ValueType> kv : _table) {
            std::cout << kv.first.member_to_string().str() << "\t\t";
            std::cout << kv.second.member_to_string().str() << std::endl;
        }
    }

    auto bucket_number(KeyType key) { return hasher(key); }

    void garbage_collect(const void *key) {}

    std::size_t get_key_size() { return sizeof(KeyType); }

    std::size_t get_value_size() { return sizeof(ValueType); }

    std::size_t get_field_size() { return ClassOf<ValueType>::size(); }

    std::size_t get_table_id() { return _table_id; }

    Database *get_database() { return _db; }
    
private:
    std::unordered_map<KeyType, std::tuple<MetaType, ValueType>> _table;
    Database *_db;
    std::size_t _table_id = 0;
};
