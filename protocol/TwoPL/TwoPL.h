/*
 * @Author: lxc
 * @Date: 05/22/2023
 */
#pragma once

#include "Config.h"
#include "Counter.h"
#include "Database.h"
#include "Global.h"
#include "Table.h"
#include "ThreadPool.h"
#include "TwoPLExecutor.h"
#include "TwoPLTransaction.h"

class TwoPL {
private:
    Database* _db;
    std::atomic<int32_t> _max_tid = 0;
    SafeQuene<TwoPLTransaction>*_ready_execute_queue, *_ready_commit_queue;

public:
    TwoPL(Database* db, SafeQuene<TwoPLTransaction>* ready_execute_queue,
          SafeQuene<TwoPLTransaction>* ready_commit_queue)
        : _db(db), _ready_execute_queue(ready_execute_queue), _ready_commit_queue(ready_commit_queue) {
        Counter::reset_counter();
    }

    TwoPL(Database* db, uint32_t start_no, SafeQuene<TwoPLTransaction>* ready_execute_queue,
          SafeQuene<TwoPLTransaction>* ready_commit_queue)
        : _db(db), _ready_execute_queue(ready_execute_queue), _ready_commit_queue(ready_commit_queue) {
        Counter::reset_counter(start_no);
    }

    void run() {
        LOG(WARNING) << "tpl transaction process starts!";
        TwoPLExecutor executor = TwoPLExecutor(_db, _ready_execute_queue, _ready_commit_queue);
        executor.execute_thread_run();
        executor.commit_thread_run();
    }
};
