/*
 * @Author: lxc
 * @Date: 06/10/2023
 */

#pragma once

#include "Global.h"

template <class T>
class SafeQueue {
private:
    mutable std::mutex _mutex;
    std::queue<std::shared_ptr<T>> _queue;
    std::condition_variable _condition;

public:
    SafeQueue() {}

    std::size_t size() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.empty();
    }

    void push(T value) {
        std::lock_guard<std::mutex> lock(_mutex);
        std::shared_ptr<T> data(std::make_shared<T>(std::move(value)));
        _queue.push(data);
        _condition.notify_one();
    }

    std::shared_ptr<T> pop() {
        std::unique_lock<std::mutex> lock(_mutex);
        if(this->_queue.empty())
            return nullptr;
        std::shared_ptr<T> value = _queue.front();
        _queue.pop();
        return value;
    }
};