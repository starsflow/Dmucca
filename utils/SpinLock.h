//
// Created by lxc on 5/9/23.
//

#ifndef DMVCCA_SPINLOCK_H
#define DMVCCA_SPINLOCK_H

#include <atomic>
#include <ostream>

class SpinLock {
public:
    // constructors
    SpinLock() = default;

    SpinLock(const SpinLock &) = delete;            // non construction-copyable
    SpinLock &operator=(const SpinLock &) = delete; // non copyable

    // Modifiers
    void lock() {
        while (_lock.test_and_set(std::memory_order_acquire));
    }

    void unlock() { _lock.clear(std::memory_order_release); }

    // friend declaration
//    friend std::ostream &operator<<(std::ostream &, const SpinLock &);

private:
    std::atomic_flag _lock = ATOMIC_FLAG_INIT;
};

#endif //DMVCCA_SPINLOCK_H
