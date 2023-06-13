/*
 * @Author: lxc
 * @Date: 05/22/2023
 */
#pragma once

#include "Global.h"
#include "Database.h"
#include "Table.h"
#include "Config.h"
#include "TwoPLTransaction.h"
#include "ThreadPool.h"
#include "Counter.h"

class TwoPL {
private:
    Database* _db;
    std::atomic<int32_t> _max_tid = 0;

    using MetaType = std::atomic<uint32_t>;
    using MetaFlag = std::atomic_flag;
public:
    TwoPL(Database* db) : _db(db) {
        Counter::reset_counter();
    }

    TwoPL(Database* db, uint32_t start_no) : _db(db) {
        Counter::reset_counter(start_no);
    }

    ITable* get_table(int table_id) {
        return static_cast<ITable *>(_db->find_table(table_id));
    }

    int32_t generate_txn_id() {
        if(_max_tid.load() < 0)
            return -1;
        _max_tid ++;
        return _max_tid.load();
    }
    
};
