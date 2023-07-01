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

template <class Transaction>
class TwoPLExecutor {
private:
    Database* _db;
    ThreadPool _execute_tp, _commit_tp;
    SafeQueue<Transaction>*_ready_execute_queue, *_ready_commit_queue;

public:
    TwoPLExecutor(Database* db, SafeQueue<Transaction>* ready_execute_queue, SafeQueue<Transaction>* ready_commit_queue)
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

    bool check_item_in_set(std::vector<RWItem> set, const void* key, std::size_t size) {
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
            is_read_lock ? TwoPLHelper::release_read_lock_bits(table, item.key)
                         : TwoPLHelper::release_write_lock_bits(table, item.key);
        }
        set.clear();
    }

    bool commit_txn(Transaction txn) {
        if (txn.write_set.size()) {
            for (auto& write_item : txn.write_set) {
                ITable* table = get_table(write_item.table_id);
                auto* value = table->search_value(write_item.key);
                if (value) {
                    memcpy(value, write_item.value, table->get_value_size());
                } else {
                    table->insert(write_item.key, 0x1, write_item.value);
                }
            }
        }

        release_read_write_locks(txn.read_set, true);
        release_read_write_locks(txn.write_set, false);
        // std::cout << "transaction " << txn.txn_id << " has unlocked" << std::endl;
        return true;
    }

    void execute_thread(ThreadPool& tp, SafeQueue<Transaction>* queue) {
        std::thread bt([this, &tp, queue]() {
            bool is_first = false;
            while (true) {
                std::shared_ptr<Transaction> txn = queue->pop();
                if (txn) {
                    if (!is_first) {
                        is_first = true;
                        LOG(WARNING) << "start time : "
                                     << std::chrono::duration_cast<std::chrono::milliseconds>(
                                            std::chrono::system_clock::now().time_since_epoch())
                                            .count();
                    }
                    tp.enqueue(
                        [](Transaction txn, TwoPLExecutor* executor) {
                            LOG(INFO) << "transaction " << txn.txn_id << " start processing!";
                            if (txn.txn_id == 0xffffffff) {
                                txn.txn_id = Counter::generate_unique_id();
                                LOG(INFO) << "the generated id is " << txn.txn_id;
                            }
                            auto res = txn.execute();
                            if (res == TransactionResult::READY_TO_COMMIT) {
                                executor->_ready_commit_queue->push(txn);
                                LOG(INFO) << "transaction " << txn.txn_id << " is to commit!";
                            } else {
                                executor->_ready_execute_queue->push(txn);
                                LOG(INFO) << "transaction " << txn.txn_id << " is to re-execute!";
                            }
                        },
                        *txn, this);
                }
            }
        });
        bt.detach();
    }

    void main_thread(ThreadPool& tp, SafeQueue<Transaction>* queue) {
        while (true) {
            std::shared_ptr<Transaction> txn = queue->pop();
            if (txn) {
                tp.enqueue(
                    [](Transaction txn, TwoPLExecutor* executor) {
                        if (executor->commit_txn(txn)) {
                            txn.status = TransactionResult::COMMIT;
                            LOG(INFO) << "transaction " << txn.txn_id << " has committed!";
                            std::cout << "transaction " << txn.txn_id << " has committed at time : "
                                      << std::chrono::duration_cast<std::chrono::milliseconds>(
                                             std::chrono::system_clock::now().time_since_epoch())
                                             .count()
                                      << std::endl;
                            // std::cout << txn.txn_id << "->";
                        } else {
                            txn.status = TransactionResult::ABORT_NORETRY;
                            LOG(INFO) << "transaction " << txn.txn_id << " has aborted with no try!";
                        }
                    },
                    *txn, this);
            }
        }
    }

    void execute_thread_run() {
        LOG(WARNING) << "execute threads start!";
        execute_thread(_execute_tp, _ready_execute_queue);
    }

    void commit_thread_run() {
        LOG(WARNING) << "commit threads start!";
        main_thread(_commit_tp, _ready_commit_queue);
    }
};