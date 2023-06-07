//
// Created by lxc on 5/9/23.
//

#pragma once
#include "ClassOf.h"
#include "Database.h"
#include "Global.h"
#include "Row.h"
#include "YSchema.h"
#include "Config.h"

class Database;

class ITable {
public:
    using MetaType = std::atomic<uint32_t>;

    virtual void* search_value(const void* key) = 0;

    virtual MetaType* search_metadata(const void* key) = 0;

    virtual bool update_metadata(const void* key, uint64_t meta) = 0;

    virtual void print_table() = 0;

    virtual bool erase(const void* key) = 0;

    virtual bool insert(const void* key, uint64_t meta, const void* value) = 0;

    virtual bool update_value(const void* key, const void* value) = 0;

};

template <class KeyType, class ValueType>
class Table : public ITable {
public:
    using MetaType = std::atomic<uint32_t>;

    Table(Database *db, std::size_t table_id)
        : _db(db), _table_id(table_id){}

    Table() {}

    virtual ~Table() = default;

    void* search_value(const void* key) {
        return this->apply_ref_with_locked_bucket<std::function<void *(Table<KeyType, ValueType> *)>>(
            [key](Table<KeyType, ValueType> *table) -> void * {
                auto result = table->_table.find(key);
                if (result == table->_table.end()) return nullptr;
                return &(std::get<1>(result->second));
            },
            get_bucket_number(key), false);
    }

    MetaType* search_metadata(const void* key) {
        return this->apply_ref_with_locked_bucket<std::function<MetaType *(Table<KeyType, ValueType> *)>>(
            [key](Table<KeyType, ValueType> *table) -> MetaType * {
                auto result = table->_table.find(key);
                if (result == table->_table.end()) return nullptr;
                return &(std::get<0>(result->second));
            },
            get_bucket_number(key), false);
    }

    bool update_metadata(const void* key, uint64_t meta) {
        return this->apply_with_locked_bucket<std::function<bool(Table<KeyType, ValueType> *)>>(
            [key, meta](Table<KeyType, ValueType> *table) -> bool {
                auto result = table->_table.find(key);
                if (result == table->_table.end()) return false;
                std::atomic_store(&std::get<0>(result->second), meta);
                return true;
            },
            get_bucket_number(key), true);
    }

    bool update_value(const void* key, const void* value) {
        return this->apply_with_locked_bucket<std::function<bool(Table<KeyType, ValueType> *)>>(
            [key, value](Table<KeyType, ValueType> *table) -> bool {
                auto result = table->_table.find(key);
                if (result == table->_table.end()) return false;
                std::get<1>(result->second) = value;
                return true;
            },
            get_bucket_number(key), true);
    }

    bool insert(const void* key, uint64_t meta, const void* value) {
        return this->apply_with_locked_bucket<std::function<bool(Table<KeyType, ValueType> *)>>(
            [key, meta, value](Table<KeyType, ValueType> *table) -> bool {
                std::atomic_store(&std::get<0>(table->_table[key]), meta);
                std::get<1>(table->_table[key]) = value;
                return true;
            },
            get_bucket_number(key), true);
    }

    bool erase(const void* key) {
        return this->apply_with_locked_bucket<
            std::function<bool(Table<KeyType, ValueType> *)>>(
            [key](Table<KeyType, ValueType> *table) -> bool {
                table->_table.erase(key);
                return true;
            },
            get_bucket_number(key), true);
    }

    void print_table() {
        std::string title_string;
        std::stringstream titles_stream(KeyType::class_to_string().str() + ";" +
                                        ValueType::class_to_string().str());
        while (getline(titles_stream, title_string, ';')) {
            std::cout << title_string << "\t\t";
        }
        std::cout << "\n";

        for (auto &[key, value] : _table) {
            std::cout << key->member_to_string().str() << "\t\t";
            std::cout << std::get<1>(value).member_to_string().str()
                      << std::endl;
        }
    }

    template <class Func>
    auto apply_with_locked_bucket(Func func, std::size_t bucket_id, bool write_lock) {
        if(write_lock)
            std::unique_lock<std::shared_mutex> lock(_mutex[bucket_id]);
        else
            std::shared_lock<std::shared_mutex> lock(_mutex[bucket_id]);
        auto result = func(static_cast<Table<KeyType, ValueType> *>(this));
        return result;
    }

    template <class Func>
    auto *apply_ref_with_locked_bucket(Func func, std::size_t bucket_id, bool write_lock) {
        if(write_lock)
            std::unique_lock<std::shared_mutex> lock(_mutex[bucket_id]);
        else
            std::shared_lock<std::shared_mutex> lock(_mutex[bucket_id]);
        auto *result = func(static_cast<Table<KeyType, ValueType> *>(this));
        return result;
    }

    auto get_bucket_number(void* key) { return hasher(static_cast<KeyType *>(key)) % BUCKET_NUMBER; }

    void garbage_collect(const void *key) {}

    std::size_t get_key_size() { return sizeof(KeyType); }

    std::size_t get_value_size() { return sizeof(ValueType); }

    std::size_t get_field_size() { return ClassOf<ValueType>::size(); }

    std::size_t get_table_id() { return _table_id; }

    Database *get_database() { return _db; }

private:
    std::unordered_map<KeyType* , std::tuple<MetaType, ValueType* >> _table;
    typename std::unordered_map<KeyType* , std::tuple<MetaType, ValueType* >>::hasher hasher;
    Database *_db;
    // SpinLock _locks[BUCKET_NUMBER];
    std::size_t _table_id = 0;
    std::size_t _bucket_number = 1;

    mutable std::shared_mutex _mutex[BUCKET_NUMBER];
};
