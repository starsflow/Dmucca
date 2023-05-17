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
private:
    std::vector<void *> _tables;
    std::vector<SpinLock> _locks;
    std::size_t _table_number = 0;

public:
    template<class KeyType, class ValueType>
    Table<KeyType, ValueType>* create_table() {
        Table<KeyType, ValueType> table(this, _table_number++);
        _tables.push_back(&table);
        return &table;
    }

    template<class Func>
    void sum(Func func, std::size_t table_id){}

    template<class Func, class KeyType, class ValueType>
    auto apply_with_locked_table(Func func, std::size_t table_id) {
        _locks[table_id].lock();
        auto result = func(static_cast<Table<KeyType, ValueType>*>(_tables[table_id]));
        _locks[table_id].unlock();
        return result;
    }

    template<class Func>
    auto &apply_ref_with_locked_table(Func func, std::size_t table_id){
        _locks[table_id].lock();
        auto &result = Func(_tables[table_id]);
        _locks[table_id].unlock();
        return result;
    }
};

#endif //DMVCCA_DATABASE_H
