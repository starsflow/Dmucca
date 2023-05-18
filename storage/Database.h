//
// Created by lxc on 5/11/23.
//

#ifndef DMVCCA_DATABASE_H
#define DMVCCA_DATABASE_H

#include "Global.h"
#include "SpainLock.h"
#include "Table.h"

template<class KeyType, class ValueType>
class Table;

class Database {
public:
    std::vector<void *> _tables;
    std::vector<SpinLock *> _locks;
    std::size_t _table_number = 0;

public:
    template<class KeyType, class ValueType>
    Table<KeyType, ValueType>* create_table() {
        LOG(INFO) << typeid(KeyType).name() << "  " << typeid(ValueType).name();
        auto *table = new Table<KeyType, ValueType>(this, _table_number++);
        _tables.push_back(reinterpret_cast<Table<KeyType, ValueType> *> (table));
        auto *lock = new SpinLock();
        _locks.push_back(lock);
        return table;
    }

    template<class Func, class KeyType, class ValueType>
    auto apply_with_locked_table(Func func, std::size_t table_id) {
        LOG(INFO) << table_id;
        _locks[table_id]->lock();
        auto result = func(static_cast<Table<KeyType, ValueType>*>(_tables[table_id]));
        _locks[table_id]->unlock();
        return result;
    }

    template<class KeyType, class ValueType>
    void print_table(){
        std::vector<std::string> table_titles;
        KeyType::key_field_to_string();
    }

    template<class Func>
    auto &apply_ref_with_locked_table(Func func, std::size_t table_id){
        _locks[table_id]->lock();
        auto &result = Func(_tables[table_id]);
        _locks[table_id]->unlock();
        return result;
    }
};

#endif //DMVCCA_DATABASE_H
