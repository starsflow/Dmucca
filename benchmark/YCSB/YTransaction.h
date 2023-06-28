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
    YRandom _random;
    YQuery _query;
    YKey _keys[YContext::keysPerTransaction];
    YValue _values[YContext::keysPerTransaction];
    bool _update[YContext::keysPerTransaction];

public:
    virtual ~YTransaction() = default;

    YTransaction(Database* db, YContext& context, SafeQuene<Transaction>* queue)
        : Transaction(db, queue),
          _context(context),
          _random(std::chrono::system_clock::now().time_since_epoch().count()),
          _query(YQuery(context, _random)) {}
    void build_transaction() {
        this->set_txn_id();

        for (auto i = 0u; i < YContext::keysPerTransaction; i++) {
            this->_keys[i] = _query.Y_KEY[i];
            this->_update[i] = _query.UPDATE[i];
            if (_query.UPDATE[i]) {
                // this->_values[i].Y_F01.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                // this->_values[i].Y_F02.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                // this->_values[i].Y_F03.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                // this->_values[i].Y_F04.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                // this->_values[i].Y_F05.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                // this->_values[i].Y_F06.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                // this->_values[i].Y_F07.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                // this->_values[i].Y_F08.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                // this->_values[i].Y_F09.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                // this->_values[i].Y_F10.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->_values[i].Y_F01.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                this->template append_write_set<YKey, YValue>(0, this->_keys[i], this->_values[i]);
            } else {
                this->template append_read_set<YKey, YValue>(0, this->_keys[i], this->_values[i]);
            }
        }
        LOG(INFO) << *this;
        this->insert_queue();
    }

    friend std::ostream& operator<<(std::ostream& os, YTransaction<Transaction>& txn){
        os << "transaction " << txn.txn_id << "'s key-value pairs are following:\n";
        for(size_t i = 0; i < YContext::keysPerTransaction; i++){
            if(txn._update[i])
                os << "w:" << txn._keys[i] << ":\t" << txn._values[i] << "\n";
            else
                os << "r:" << txn._keys[i] << ":\t" << txn._values[i] << "\n";
        }
        os << "transaction " << txn.txn_id << "'s key-value pairs are above.\n";
        return os;
    }

    void reset_query() { this->_query = YQuery(_context); }
};
