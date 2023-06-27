/*
 * @Author: lxc
 * @Date: 05/09/2023
 */
#pragma once

#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <jemalloc/jemalloc.h>

enum class TransactionResult {READY, COMMIT, READY_TO_COMMIT, ABORT, ABORT_NORETRY };
