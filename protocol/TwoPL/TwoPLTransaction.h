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
    Database* _db;

public:
    TwoPLTransaction(Database* db) : _db(db) {
        status = TransactionResult::READY;
        txn_id = 0xffffffff;
    }

    ~TwoPLTransaction() {
        read_set.clear();
        write_set.clear();
    }

    virtual TransactionResult execute() = 0;

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
    ValueType append_read_set(std::uint32_t table_id, KeyType key) {
        KeyType* new_key = new KeyType(key);
        RWItem* read_item = get_read_write_item<KeyType>(read_set, new_key);
        RWItem* write_item = get_read_write_item<KeyType>(read_set, new_key);
        LOG(INFO) << (write_item == nullptr);
        if (write_item) {
            auto* new_value = new ValueType(*static_cast<ValueType*>(write_item->value));
            if (read_item) {
                read_item->value = new_value;
            } else {
                read_set.emplace_back(RWItem(new_key, new_value, table_id));
            }
            return *new_value;
        } else {
            if (read_item) {
                return *static_cast<ValueType*>(read_item->value);
            } else {
                ITable* table = get_table(table_id);
                read_set.emplace_back(RWItem(new_key, nullptr, table_id));
                return *static_cast<ValueType*>(table->search_value(&key));
            }
        }
    }

    template <class KeyType, class ValueType>
    void append_write_set(std::uint32_t table_id, KeyType key, ValueType value) {
        KeyType* new_key = new KeyType(key);
        ValueType* new_value = new ValueType(value);
        RWItem* write_item = get_read_write_item<KeyType>(write_set, new_key);
        if (!write_item) {
            write_set.emplace_back(RWItem(new_key, new_value, table_id));
        } else {
            write_item->value = new_value;
        }
    }

    void release_read_write_locks(std::vector<RWItem> set, bool is_read_lock) {
        for (auto item : set) {
            ITable* table = get_table(item.table_id);
            is_read_lock ? TwoPLHelper::release_read_lock_bits(table, item.key)
                         : TwoPLHelper::release_write_lock_bits(table, item.key);
        }
        set.clear();
    }

    template <class KeyType, class ValueType>
    std::pair<bool, ValueType> read(std::uint32_t table_id, KeyType key) {
        // request locks
        auto* new_key = new KeyType(key);
        auto* table = get_table(table_id);
        bool can_read_locked = TwoPLHelper::set_read_lock_bits(table, new_key, txn_id);
        if (!can_read_locked) {
            LOG(INFO) << key << " has not be locked";
            release_read_write_locks(read_set, true);
            release_read_write_locks(write_set, false);
            return std::pair<bool, ValueType>(false, ValueType());
        }
        LOG(INFO) << key << " has be locked";
        ValueType res = this->append_read_set<KeyType, ValueType>(table_id, key);
        return std::pair<bool, ValueType>(true, res);
    }

    template <class KeyType, class ValueType>
    bool update(std::uint32_t table_id, KeyType key, ValueType value) {
        // request locks
        auto* table = get_table(table_id);
        auto* new_key = new KeyType(key);
        bool can_write_locked = TwoPLHelper::set_write_lock_bits(table, new_key, txn_id);
        if (!can_write_locked) {
            LOG(INFO) << key << " has not be locked";
            release_read_write_locks(read_set, true);
            release_read_write_locks(write_set, false);
            return false;
        }
        this->append_write_set<KeyType, ValueType>(table_id, key, value);
        return true;
    }
};
