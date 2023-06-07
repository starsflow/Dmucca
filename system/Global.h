/*
 * @Author: lxc
 * @Date: 05/09/2023
 */
#pragma once

#include <iostream>
#include <thread>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <memory>
#include <sstream>
#include <atomic>
#include <shared_mutex>
#include <chrono>

#include <glog/logging.h>
#include <gtest/gtest.h>

enum class TransactionResult { COMMIT, READY_TO_COMMIT, ABORT, ABORT_NORETRY };
