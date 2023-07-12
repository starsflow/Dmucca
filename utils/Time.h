/*
 * @Author: lxc
 * @Date: 07/12/2023
 */

#pragma once

#include "Global.h"

class Time {
public:
    static uint64_t now() {
        auto now = std::chrono::system_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
        return static_cast<uint64_t>(ns.count());
    }
};