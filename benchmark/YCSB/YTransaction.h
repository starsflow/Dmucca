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
#include "Time.h"

template <class Transaction>
class YTransaction : public Transaction {
public:
    YRandom random;
    YQuery query;
    std::vector<YKey> keys;
    std::vector<YValue> values;
    std::vector<bool> is_update;

public:
    virtual ~YTransaction() = default;

    YTransaction(Database* db)
        : Transaction(db), random(Time::now()), query() {}

    void insert_queue(SafeQueue<YTransaction<Transaction>>* queue) { queue->push(*this); }

    TransactionResult execute() override {
        this->keys.clear();
        this->values.clear();
        this->read_set.clear();
        this->write_set.clear();
        LOG(INFO) << query;
        for (auto i = 0u; i < query.keys_per_transaction; i++) {
            this->keys.emplace_back(query.Y_KEY[i]);
            this->is_update.emplace_back(query.UPDATE[i]);
            if (query.UPDATE[i]) {
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
                YValue value{random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE)};
                this->values.emplace_back(value);
                if (!this->template update<YKey, YValue>(0, this->keys[i], this->values[i])) {
                    this->status = TransactionResult::ABORT;
                    return TransactionResult::ABORT;
                }
                // this->template append_write_set<YKey, YValue>(0, this->_keys[i], this->_values[i]);
            } else {
                this->values.emplace_back(YValue());
                auto res = this->template read<YKey, YValue>(0, this->keys[i]);
                if (res.first) {
                    // nothing need to do with res.second;
                } else {
                    this->status = TransactionResult::ABORT;
                    return TransactionResult::ABORT;
                }
                // this->template append_read_set<YKey, YValue>(0, this->_keys[i], this->_values[i]);
            }
        }
        this->status = TransactionResult::READY_TO_COMMIT;
        return TransactionResult::READY_TO_COMMIT;
    }
};
