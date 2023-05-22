//
// Created by lxc on 5/9/23.
//

#ifndef DMVCCA_GLOBAL_H
#define DMVCCA_GLOBAL_H

#include <iostream>
#include <thread>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <memory>
#include <sstream>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <atomic>
#include <shared_mutex>

enum class TransactionResult { COMMIT, READY_TO_COMMIT, ABORT, ABORT_NORETRY };

#endif //DMVCCA_GLOBAL_H
