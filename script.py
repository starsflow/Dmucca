'''
Author: lxc
Date: 06/10/2023
'''

strings = [
"#include <iostream>",
"#include <thread>",
"#include <cmath>",
"#include <unordered_set>",
"#include <unordered_map>",
"#include <functional>",
"#include <memory>",
"#include <sstream>",
"#include <atomic>",
"#include <shared_mutex>",
"#include <chrono>",
"#include <condition_variable>",
"#include <future>",
"#include <mutex>",
"#include <queue>",
"#include <stdexcept>",
"#include <vector>",
"#include <variant>"
]

sorted_strings = sorted(strings)
print('\n'.join(sorted_strings))
