/*
 * @Author: lxc
 * @Date: 06/10/2023
 */

 #pragma once

#include "Config.h"
#include "Global.h"
#include "SafeQueue.h"
#include "ThreadPool.h"
#include "TwoPLTransaction.h"

class TwoPLExecutor {
private:
    ThreadPool _tp;
    SafeQuene<TwoPLTransaction> _queue;

public:
    TwoPLExecutor() : _tp(THREAD_CNT) {}

    void insert_queue(TwoPLTransaction& txn) {
        _queue.push(txn);
    }

    void handle_txns() {
        
    }
};