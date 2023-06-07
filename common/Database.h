/*
 * @Author: lxc
 * @Date: 05/11/2023
 */

#pragma once

#include "Global.h"
#include "SpinLock.h"
#include "Table.h"
#include "Config.h"

template<class KeyType, class ValueType>
class Table;

class Database {
public:
    std::vector<void *> _tables;
    std::vector<SpinLock *> _locks;
    std::size_t _table_number = 0;

public:
    template<class KeyType, class ValueType>
    void* create_table() {
        ITable *table = new Table<KeyType, ValueType>(this, _table_number++);
        _tables.push_back(table);
        auto *lock = new SpinLock();
        _locks.push_back(lock);
        return table;
    }

    void* find_table(std::size_t table_id){
        if(table_id >= _tables.size())
            return nullptr;
        return _tables[table_id];
    }

    //lock the totoal table
    template<class Func, class KeyType, class ValueType>
    auto apply_with_locked_table(Func func, std::size_t table_id) {
        _locks[table_id]->lock();
        auto result = func(static_cast<Table<KeyType, ValueType>*>(_tables[table_id]));
        _locks[table_id]->unlock();
        return result;
    }

    template<class Func, class KeyType, class ValueType>
    auto* apply_ref_with_locked_table(Func func, std::size_t table_id){
        _locks[table_id]->lock();
        auto* result = func(static_cast<Table<KeyType, ValueType>*>(_tables[table_id]));
        _locks[table_id]->unlock();
        return result;
    }
};
