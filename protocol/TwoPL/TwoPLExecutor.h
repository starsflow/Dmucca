/*
 * @Author: lxc
 * @Date: 06/10/2023
 */

#pragma once

#include "Config.h"
#include "Database.h"
#include "Global.h"
#include "SafeQueue.h"
#include "Table.h"
#include "ThreadPool.h"
#include "TwoPLTransaction.h"

class TwoPLExecutor {
private:
    ThreadPool _tp;
    SafeQuene<TwoPLTransaction> _queue;
    Database* _db;

public:
    TwoPLExecutor(Database* db) : _tp(THREAD_CNT), _db(db) {}

    ITable* get_table(int table_id) {
        return static_cast<ITable*>(_db->find_table(table_id));
    }

    void insert_queue(TwoPLTransaction& txn) { _queue.push(txn); }

    bool check_item_in_set(std::vector<RWItem> &set, const void* key) {
        for(int i = 0; i < set.size(); i++){
            if(set[i].key == key) {
                return true; 
            }
        }
        return false;
    }

    bool request_all_locks(TwoPLTransaction& txn) {
        for (auto read_item : txn.read_set) {
            ITable* table = get_table(read_item.table_id);
            bool can_read_locked = TwoPLHelper::set_read_lock_bit(table, read_item.key, txn.txn_id);
            //all write locks of the transaction haven't been added
            if(!can_read_locked) {
                return false;
            }
        }

        for(auto write_item : txn.write_set) {
            ITable* table = get_table(write_item.table_id);
            bool can_write_locked = TwoPLHelper::set_write_lock_bit(table, write_item.key, write_item.table_id);
            //check whether the record is occupied by other transaction's read or write lock
            //executing here indicates that all read locks have been successfully added
            
            //how to get the concurrnent read transactions' number
            if(!can_write_locked && check_item_in_set(txn.read_set, write_item.key)) {
                return false;
            }
        }

        return true;
    }

    bool commit_txn(TwoPLTransaction& txn) {
        for (auto& read_item : txn.read_set) {
            if (!read_item.value) {
                ITable* table = get_table(read_item.table_id);
                read_item.value = je_malloc(table->get_value_size());
                auto* value = table->search_value(read_item.key);
                if(!value)
                    memcpy(read_item.value, value, table->get_value_size());
                else
                    return false;
            }
        }

        for(auto& write_item : txn.write_set) {
            ITable* table = get_table(write_item.table_id);
            auto* value = table->search_value(write_item.key);
            if(!value)
                memcpy(value, write_item.value, table->get_value_size());
            else
                table->insert(write_item.key, 0x1, write_item.value);
        }

        return true;
    }

    void thread_run() {
        while(true) {
            
        }
    }
};