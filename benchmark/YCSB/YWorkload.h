/*
 * @Author: lxc
 * @Date: 06/07/2023
 */

#pragma once

#include "Database.h"
#include "Global.h"
#include "TwoPL/TwoPLTransaction.h"
#include "YTransaction.h"

template <class Transaction>
class YWorkload {
public:
    using TransactionType = YTransaction<Transaction>;

private:
    Database* _db;
    SafeQueue<TransactionType>* _queue;

public:
    YWorkload(Database* db, SafeQueue<TransactionType>* queue) : _db(db), _queue(queue) {}

    std::unique_ptr<TransactionType> next_transaction() {
        auto p = std::make_unique<TransactionType>(_db);
        return p;
    }

    void generate_workload_thread(std::size_t txn_num) {
        std::thread bt_thread([this, txn_num]() {
            for (size_t i = 0; i < txn_num; i++) {
                auto p = this->next_transaction();
                p->insert_queue(_queue);
            }
        });
        bt_thread.detach();
    }
};