/*
 * @Author: lxc
 * @Date: 05/11/2023
 */
#pragma once

#include "Global.h"
#include "SafeQueue.h"
#include "YContext.h"
#include "YQuery.h"
#include "YRandom.h"
#include "YSchema.h"
#include "YTable.h"

template <class Transaction>
class YTransaction : public Transaction {
public:
    YContext _context;
    YQuery _query;
    YRandom _random;
    YKey _keys[YContext::keysPerTransaction];
    YValue _values[YContext::keysPerTransaction];

public:
    virtual ~YTransaction() = default;

    YTransaction(Database* db, YContext& context, SafeQuene<Transaction>* queue)
        : Transaction(db, queue), _context(context), _query(YQuery(context)) {}

    void build_transaction() {
        this->set_txn_id();

        for (auto i = 0u; i < YContext::keysPerTransaction; i++) {
            this->_keys[i] = _query.Y_KEY[i];
            if (_query.UPDATE[i]) {
                this->_values[i].Y_F01.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->_values[i].Y_F02.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->_values[i].Y_F03.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->_values[i].Y_F04.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->_values[i].Y_F05.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->_values[i].Y_F06.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->_values[i].Y_F07.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->_values[i].Y_F08.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->_values[i].Y_F09.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->_values[i].Y_F10.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->template append_write_set<YKey, YValue>(0, &this->_keys[i], &this->_values[i]);
            } else {
                this->template append_read_set<YKey, YValue>(0, &this->_keys[i], &this->_values[i]);
            }
        }
        LOG(INFO) << "transaction " << this->txn_id << " has built!";
        this->insert_queue();
    }

    void reset_query() { this->_query = YQuery(_context); }
};
