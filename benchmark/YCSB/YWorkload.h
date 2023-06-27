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
private:
    Database* _db;
    YContext _context;
    SafeQuene<Transaction>* _queue;

public:
    YWorkload(Database* db, YContext& context, SafeQuene<Transaction>* queue)
        : _db(db), _context(context), _queue(queue) {}

    std::unique_ptr<YTransaction<Transaction>> next_transaction() {
        std::unique_ptr<YTransaction<Transaction>> p =
            std::make_unique<YTransaction<Transaction>>(_db, _context, _queue);
        return p;
    }

    void generate_workload_thread(std::size_t txn_num) {
        std::thread bt_thread([this, txn_num]() {
            for (size_t i = 0; i < txn_num; i++) {
                auto p = this->next_transaction();
                p->build_transaction();
            }
        });
        bt_thread.detach();
    }
};