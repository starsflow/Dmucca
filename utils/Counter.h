/*
 * @Author: lxc
 * @Date: 06/13/2023
 */

#pragma once

#include "Global.h"

class Counter {
private:
    static std::atomic<uint32_t> _counter;

public:
    static void reset_counter() {
        _counter.store(0);
    }

    static void reset_counter(uint32_t number){
        _counter.store(number);
    }

    static uint32_t generate_unique_id() {
        _counter.fetch_add(1);
        return _counter.load();
    }
};