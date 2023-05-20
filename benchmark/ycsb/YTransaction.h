//
// Created by lxc on 5/11/23.
//

#ifndef DMVCCA_YTRANSACTION_H
#define DMVCCA_YTRANSACTION_H

#include "Global.h"
#include "YContext.h"
#include "YTable.h"

template <class Transaction>
class ReadModifyWrite : public Transaction {
private:
    YDatabase &_db;
    DatabaseType &db;
    const ContextType &context;
    RandomType &random;
    Storage &storage;
    std::size_t partition_id;
    YCSBQuery<keys_num> query;

public:
    using DatabaseType = Database;
    using ContextType = typename DatabaseType::ContextType;
    using RandomType = typename DatabaseType::RandomType;
    using StorageType = Storage;

    static constexpr std::size_t keys_num = 10;

    ReadModifyWrite(std::size_t coordinator_id, std::size_t partition_id,
                    DatabaseType &db, const ContextType &context,
                    RandomType &random, Partitioner &partitioner,
                    Storage &storage)
            : Transaction(coordinator_id, partition_id, partitioner), db(db),
              context(context), random(random), storage(storage),
              partition_id(partition_id),
              query(makeYCSBQuery<keys_num>()(context, partition_id, random)) {}

    virtual ~ReadModifyWrite() override = default;

    TransactionResult execute(std::size_t worker_id) override {

        DCHECK(context.keysPerTransaction == keys_num);

        int ycsbTableID = ycsb::tableID;

        for (auto i = 0u; i < keys_num; i++) {
            auto key = query.Y_KEY[i];
            storage.ycsb_keys[i].Y_KEY = key;
            if (query.UPDATE[i]) {
                this->search_for_update(ycsbTableID, context.getPartitionID(key),
                                        storage.ycsb_keys[i], storage.ycsb_values[i]);
            } else {
                this->search_for_read(ycsbTableID, context.getPartitionID(key),
                                      storage.ycsb_keys[i], storage.ycsb_values[i]);
            }
        }

        if (this->process_requests(worker_id)) {
            return TransactionResult::ABORT;
        }

        for (auto i = 0u; i < keys_num; i++) {
            auto key = query.Y_KEY[i];
            if (query.UPDATE[i]) {

                if (this->execution_phase) {
                    RandomType local_random;
                    storage.ycsb_values[i].Y_F01.assign(
                            local_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    storage.ycsb_values[i].Y_F02.assign(
                            local_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    storage.ycsb_values[i].Y_F03.assign(
                            local_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    storage.ycsb_values[i].Y_F04.assign(
                            local_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    storage.ycsb_values[i].Y_F05.assign(
                            local_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    storage.ycsb_values[i].Y_F06.assign(
                            local_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    storage.ycsb_values[i].Y_F07.assign(
                            local_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    storage.ycsb_values[i].Y_F08.assign(
                            local_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    storage.ycsb_values[i].Y_F09.assign(
                            local_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                    storage.ycsb_values[i].Y_F10.assign(
                            local_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
                }
                this->update(ycsbTableID, context.getPartitionID(key),
                             storage.ycsb_keys[i], storage.ycsb_values[i]);
            }
        }

        return TransactionResult::READY_TO_COMMIT;
    }

    void reset_query() override {
        query = makeYCSBQuery<keys_num>()(context, partition_id, random);
    }
};

#endif //DMVCCA_YTRANSACTION_H
