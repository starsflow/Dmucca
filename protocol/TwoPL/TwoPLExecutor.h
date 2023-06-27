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
    Database* _db;
    ThreadPool _execute_tp, _commit_tp;
    SafeQuene<TwoPLTransaction>*_ready_execute_queue, *_ready_commit_queue;

public:
    TwoPLExecutor(Database* db, SafeQuene<TwoPLTransaction>* ready_execute_queue,
                  SafeQuene<TwoPLTransaction>* ready_commit_queue)
        : _db(db),
          _execute_tp(THREAD_CNT / 2),
          _commit_tp(THREAD_CNT / 2),
          _ready_execute_queue(ready_execute_queue),
          _ready_commit_queue(ready_commit_queue) {}

    ITable* get_table(int table_id) {
        void* table = _db->find_table(table_id);
        return static_cast<ITable*>(table);
    }

    // void insert_queue(TwoPLTransaction& txn) { _queue.push(txn); }

    bool check_item_in_set(std::vector<RWItem>& set, const void* key, std::size_t size) {
        for (std::size_t i = 0; i < set.size(); i++) {
            if (!memcmp(set[i].key, key, size)) {
                return true;
            }
        }
        return false;
    }

    void release_read_write_locks(std::vector<RWItem> set, bool is_read_lock) {
        for (auto item : set) {
            ITable* table = get_table(item.table_id);
            is_read_lock ? TwoPLHelper::resize_read_lock_number_bits(table, item.key, false)
                         : TwoPLHelper::reset_write_lock_bit(table, item.key);
        }
        set.clear();
    }

    bool request_all_locks(TwoPLTransaction& txn) {
        std::vector<RWItem> locked_read_set, locked_write_set;
        for (auto write_item : txn.write_set) {
            ITable* table = get_table(write_item.table_id);
            bool can_write_locked = TwoPLHelper::set_write_lock_bit(table, write_item.key, write_item.table_id);
            if (!can_write_locked) {
                release_read_write_locks(locked_write_set, false);
                return false;
            } else {
                locked_write_set.push_back(write_item);
            }
        }

        for (auto read_item : txn.read_set) {
            ITable* table = get_table(read_item.table_id);
            bool can_read_locked = TwoPLHelper::set_read_lock_bit(table, read_item.key, txn.txn_id);
            // check whether the record is occupied by other transaction's read or write lock
            // executing here indicates that all read locks have been successfully added

            // all write locks of the transaction haven been added
            if (!read_item.value && !can_read_locked) {
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
                if (value)
                    memcpy(read_item.value, value, table->get_value_size());
                else
                    return false;
            }
        }

        for (auto& write_item : txn.write_set) {
            ITable* table = get_table(write_item.table_id);
            auto* value = table->search_value(write_item.key);
            if (value)
                memcpy(value, write_item.value, table->get_value_size());
            else
                table->insert(write_item.key, 0x1, write_item.value);
        }

        release_read_write_locks(txn.read_set, true);
        release_read_write_locks(txn.write_set, true);
        return true;
    }

    void execute_thread(ThreadPool& tp, SafeQuene<TwoPLTransaction>* queue) {
        std::thread bt([this, &tp, queue]() {
            TwoPLTransaction* txn;
            while (true) {
                if ((txn = queue->pop())) {
                    tp.enqueue(
                        [txn](TwoPLExecutor* executor) {
                            LOG(INFO) << "transaction " << txn->txn_id << " start processing!";
                            if (executor->request_all_locks(*txn)) {
                                txn->status = TransactionResult::READY_TO_COMMIT;
                                executor->_ready_commit_queue->push(*txn);
                                LOG(INFO) << "transaction " << txn->txn_id << " is to commit!";
                            } else {
                                txn->status = TransactionResult::ABORT;
                                executor->_ready_execute_queue->push(*txn);
                                LOG(INFO) << "transaction " << txn->txn_id << " is to re-execute!";
                            }
                        },
                        this);
                }
            }
        });
        bt.detach();
    }

    void main_thread(ThreadPool& tp, SafeQuene<TwoPLTransaction>* queue) {
        TwoPLTransaction* txn;
        while (true) {
            if ((txn = queue->pop())) {
                tp.enqueue(
                    [txn](TwoPLExecutor* executor) {
                        if (executor->commit_txn(*txn)) {
                            txn->status = TransactionResult::COMMIT;
                            LOG(INFO) << "transaction " << txn->txn_id << " has committed!";
                        } else {
                            txn->status = TransactionResult::ABORT_NORETRY;
                            LOG(INFO) << "transaction " << txn->txn_id << " has aborted with no try!";
                        }
                    },
                    this);
            }
        }
    }

    // void base_thread_1(ThreadPool& tp, SafeQuene<TwoPLTransaction>* queue) {
    //     std::thread bt([this, &tp, queue]() {
    //         while (true) {
    //             if (!queue->empty()) {
    //                 LOG(INFO) << "before pop the size of queue1 is " << queue->size();
    //                 tp.enqueue(
    //                 [queue](TwoPLExecutor* executor) {
    //                     LOG(INFO) << "the size of queue1 is " << queue->size();
    //                     TwoPLTransaction txn = queue->pop();
    //                     LOG(INFO) << "after pop the size of queue1 is " << queue->size();
    //                     LOG(INFO) << "transaction " << txn.txn_id << " start processing!";
    //                     if (executor->request_all_locks(txn)) {
    //                         txn.status = TransactionResult::READY_TO_COMMIT;
    //                         executor->_ready_commit_queue->push(txn);
    //                         LOG(INFO) << "transaction " << txn.txn_id << " is to commit!";
    //                     } else {
    //                         txn.status = TransactionResult::ABORT;
    //                         executor->_ready_execute_queue->push(txn);
    //                         LOG(INFO) << "transaction " << txn.txn_id << " is to re-execute!";
    //                     }
    //                 },
    //                 this);
    //             }
    //         }
    //     });
    //     bt.detach();
    // }

    void execute_thread_run() {
        LOG(WARNING) << "execute threads start!";
        execute_thread(_execute_tp, _ready_execute_queue);
    }

    void commit_thread_run() {
        LOG(WARNING) << "commit threads start!";
        main_thread(_commit_tp, _ready_commit_queue);
    }
};