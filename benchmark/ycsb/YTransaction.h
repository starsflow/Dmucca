/*
 * @Author: lxc
 * @Date: 05/11/2023
 */
#pragma once

#include "Global.h"
#include "YContext.h"
#include "YQuery.h"
#include "YRandom.h"
#include "YSchema.h"
#include "YTable.h"

template <class Transaction>
class YTransaction : public Transaction {
private:
    const YContext &_context;
    YRandom &_random;
    YQuery _query;
    YKey _keys[YContext::keysPerTransaction];
    YValue _values[YContext::keysPerTransaction];

public:
    static constexpr std::size_t keys_num = YContext::keysPerTransaction;
    virtual ~YTransaction() override = default;

    TransactionResult execute(std::size_t worker_id) override {
        DCHECK(YContext::keysPerTransaction == keys_num);

        for (auto i = 0u; i < keys_num; i++) {
            this->_keys[i] = _query.Y_KEY[i];
            if (_query.UPDATE[i]) {
                // read is needed before write the object
                this->search_for_update(0, &this->_keys[i], &this->_values[i]);
            } else {
                this->search_for_read(0, &this->_keys[i], &this->_values[i]);
            }
        }

        if (this->process_requests(worker_id)) {
            return TransactionResult::ABORT;
        }

        for (auto i = 0u; i < keys_num; i++) {
            if (_query.UPDATE[i]) {
                if (this->execution_phase) {
                    this->_values[i].Y_F01.assign(Random::a_string(
                        YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    this->_values[i].Y_F02.assign(Random::a_string(
                        YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    this->_values[i].Y_F03.assign(Random::a_string(
                        YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    this->_values[i].Y_F04.assign(Random::a_string(
                        YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    this->_values[i].Y_F05.assign(Random::a_string(
                        YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    this->_values[i].Y_F06.assign(Random::a_string(
                        YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    this->_values[i].Y_F07.assign(Random::a_string(
                        YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    this->_values[i].Y_F08.assign(Random::a_string(
                        YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    this->_values[i].Y_F09.assign(Random::a_string(
                        YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    this->_values[i].Y_F10.assign(Random::a_string(
                        YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                }
                this->update(0, &this->_keys[i], &this->_values[i]);
            }
        }

        return TransactionResult::READY_TO_COMMIT;
    }

    void reset_query() override {
        this->_query = YQuery();
    }
};
