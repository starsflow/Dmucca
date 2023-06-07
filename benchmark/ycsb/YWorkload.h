/*
 * @Author: lxc
 * @Date: 06/07/2023
 */

#pragma once

#include "Database.h"
#include "Global.h"
#include "YTransaction.h"

template <class Transaction>
class YWorkload {
private:
    Database* _db;
public:
    YWorkload(Database* db) : _db(db) {} 

    YTransaction<Transaction>* next_transaction() {
        std::unique_ptr<Transaction> p = std::make_unique<YTransaction<Transaction>>(_db);
        return p;
    }
};