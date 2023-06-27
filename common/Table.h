//
// Created by lxc on 5/9/23.
//

#pragma once
#include "ClassOf.h"
#include "Database.h"
#include "Global.h"
#include "Row.h"
#include "YCSB/YSchema.h"
#include "Config.h"

class Database;

class ITable {
public:
    using MetaType = std::atomic<uint32_t>;

    virtual void* search_value(void* key) = 0;

    virtual MetaType* search_metadata(void* key) = 0;

    virtual bool update_metadata(void* key, uint64_t meta) = 0;

    virtual bool erase(void* key) = 0;

    virtual bool insert(void* key, uint64_t meta, void* value) = 0;

    virtual bool update_value(void* key, void* value) = 0;

    virtual std::size_t get_key_size() = 0;

    virtual std::size_t get_value_size() = 0;

    virtual std::size_t get_field_size() = 0;
    
    virtual std::size_t get_table_id() = 0;  
};

template <class KeyType, class ValueType>
class Table : public ITable {
public:
    using MetaType = std::atomic<uint32_t>;

    Table(Database *db, std::size_t table_id)
        : _db(db), _table_id(table_id){}

    Table() {}

    virtual ~Table() = default;

    void* search_value(void* key_p) override {
        auto key = *static_cast<KeyType*>(key_p);
        return this->apply_ref_with_locked_bucket<std::function<void *(Table<KeyType, ValueType> *)>>(
            [key](Table<KeyType, ValueType> *table) -> void * {
                auto result = table->_table.find(key);
                if (result == table->_table.end()) return nullptr;
                return std::get<1>(result->second);
            },
            get_bucket_number(key), false);
    }

    MetaType* search_metadata(void* key_p) override {
        auto key = *static_cast<KeyType*>(key_p);
        return this->apply_ref_with_locked_bucket<std::function<MetaType *(Table<KeyType, ValueType> *)>>(
            [key](Table<KeyType, ValueType> *table) -> MetaType * {
                auto result = table->_table.find(key);
                if (result == table->_table.end()) return nullptr;
                return &(std::get<0>(result->second));
            },
            get_bucket_number(key), false);
    }

    bool update_metadata(void* key_p, uint64_t meta) override {
        auto key = *static_cast<KeyType*>(key_p);
        return this->apply_with_locked_bucket<std::function<bool(Table<KeyType, ValueType> *)>>(
            [key, meta](Table<KeyType, ValueType> *table) -> bool {
                auto result = table->_table.find(key);
                if (result == table->_table.end()) return false;
                std::atomic_store(&std::get<0>(result->second), meta);
                return true;
            },
            get_bucket_number(key), true);
    }

    bool update_value(void* key_p, void* value_v) override {
        auto key = *static_cast<KeyType*>(key_p);
        auto* value = static_cast<ValueType*>(value_v);
        return this->apply_with_locked_bucket<std::function<bool(Table<KeyType, ValueType> *)>>(
            [key, value](Table<KeyType, ValueType> *table) -> bool {
                auto result = table->_table.find(key);
                if (result == table->_table.end()) return false;
                std::get<1>(result->second) = value;
                return true;
            },
            get_bucket_number(key), true);
    }

    bool insert(void* key_p, uint64_t meta, void* value_v) override {
        auto key = *static_cast<KeyType*>(key_p);
        auto* value = static_cast<ValueType*>(value_v);
        return this->apply_with_locked_bucket<std::function<bool(Table<KeyType, ValueType> *)>>(
            [key, meta, value](Table<KeyType, ValueType> *table) -> bool {
                std::atomic_store(&std::get<0>(table->_table[key]), meta);
                std::get<1>(table->_table[key]) = value;
                return true;
            },
            get_bucket_number(key), true);
    }

    bool erase(void* key_p) override {
        auto key = *static_cast<KeyType*>(key_p);
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
            std::cout << key.member_to_string().str() << "\t\t";
            std::cout << std::get<1>(value)->member_to_string().str()
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

    auto get_bucket_number(KeyType key) { return hasher(key) % BUCKET_NUMBER; }

    void garbage_collect(void *key) {}

    std::size_t get_key_size() override { return sizeof(KeyType); }

    std::size_t get_value_size() override { return sizeof(ValueType); }

    std::size_t get_field_size() override { return ClassOf<ValueType>::size(); }

    std::size_t get_table_id() override { return _table_id; }

    Database *get_database() { return _db; }

private:
    std::unordered_map<KeyType, std::tuple<MetaType, ValueType* >> _table;
    typename std::unordered_map<KeyType, std::tuple<MetaType, ValueType* >>::hasher hasher;
    Database *_db;
    // SpinLock _locks[BUCKET_NUMBER];
    std::size_t _table_id = 0;
    std::size_t _bucket_number = 1;

    mutable std::shared_mutex _mutex[BUCKET_NUMBER];
};
