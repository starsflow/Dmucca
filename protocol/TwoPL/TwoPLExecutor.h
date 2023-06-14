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
    ThreadPool _execute_tp, _commit_tp;
    SafeQuene<TwoPLTransaction> _ready_execute_queue, _ready_commit_queue;
    Database* _db;

public:
    TwoPLExecutor(Database* db) : _execute_tp(THREAD_CNT / 2), _commit_tp(THREAD_CNT / 2), _db(db) {}

    ITable* get_table(int table_id) {
        return static_cast<ITable*>(_db->find_table(table_id));
    }

    // void insert_queue(TwoPLTransaction& txn) { _queue.push(txn); }

    bool check_item_in_set(std::vector<RWItem> &set, const void* key) {
        for(int i = 0; i < set.size(); i++){
            if(set[i].key == key) {
                return true; 
            }
        }
        return false;
    }

    void release_read_write_locks(std::vector<RWItem> set, bool is_read_lock) {
        for(auto item : set) {
            ITable* table = get_table(item.table_id);
            is_read_lock ?  TwoPLHelper::resize_read_lock_number_bits(table, item.key, false) : 
                            TwoPLHelper::reset_write_lock_bit(table, item.key);
        }
        set.clear();
    }

    bool request_all_locks(TwoPLTransaction& txn) {
        std::vector<RWItem> locked_read_set, locked_write_set;
        for(auto write_item : txn.write_set) {
            ITable* table = get_table(write_item.table_id);
            bool can_write_locked = TwoPLHelper::set_write_lock_bit(table, write_item.key, write_item.table_id);
            if(!can_write_locked) {
                release_read_write_locks(locked_write_set, false);
                return false;
            } else {
                locked_write_set.push_back(write_item);
            }
        }

        for (auto read_item : txn.read_set) {
            ITable* table = get_table(read_item.table_id);
            bool can_read_locked = TwoPLHelper::set_read_lock_bit(table, read_item.key, txn.txn_id);
            //check whether the record is occupied by other transaction's read or write lock
            //executing here indicates that all read locks have been successfully added

            //all write locks of the transaction haven been added
            if(!read_item.value && !can_read_locked) {
                release_read_write_locks(locked_write_set, false);
                release_read_write_locks(locked_read_set, true);
                return false;
            } else {
                locked_read_set.push_back(read_item);
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

        release_read_write_locks(txn.read_set, true);
        release_read_write_locks(txn.write_set, true);
        return true;
    }

    void execute_thread_run() {
        while(true) {
            _execute_tp.enqueue(
                [this] {
                    if(!this->_ready_execute_queue.empty()) {
                        TwoPLTransaction txn = _ready_execute_queue.pop();
                        if(this->request_all_locks(txn)) {
                            txn.status = TransactionResult::READY_TO_COMMIT;
                            this->_ready_commit_queue.push(txn);
                        } else {
                            txn.status = TransactionResult::ABORT;
                            this->_ready_execute_queue.push(txn);
                        }
                    }
                }
            );
        }
    }

    void commit_thread_run() {
        while(true) {
            _commit_tp.enqueue(
                [this] {
                    if(!this->_ready_commit_queue.empty()) {
                        TwoPLTransaction txn = _ready_commit_queue.pop();
                        if(this->commit_txn(txn)) {
                            txn.status = TransactionResult::COMMIT;
                        } else {
                            txn.status = TransactionResult::ABORT_NORETRY;
                        }
                    }
                }
            );
        }
    }
};