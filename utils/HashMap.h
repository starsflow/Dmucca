//
// Created by lxc on 5/9/23.
//

#ifndef DMVCCA_HASHMAP_H
#define DMVCCA_HASHMAP_H

#include <atomic>
#include <unordered_map>

#include "Global.h"
#include "SpinLock.h"

template <std::size_t N, class KeyType, class ValueType>
class HashMap {
public:
    using HashMapType = std::unordered_map<KeyType, ValueType>;
    using HasherType = typename HashMapType::hasher;

public:
    bool remove(const KeyType &key) {
        return _apply(
            [&key](HashMapType &map) {
                auto it = map.find(key);
                if (it == map.end()) {
                    return false;
                } else {
                    map.erase(it);
                    return true;
                }
            },
            _bucket_number(key));
    }

    bool contain(const KeyType &key) {
        return _apply(
            [&key](const HashMapType &map) {
                return map.find(key) != map.end();
            },
            _bucket_number(key));
    }

    bool insert(const KeyType &key, const ValueType &value) {
        return _apply(
            [&key, &value](HashMapType &map) {
                if (map.find(key) != map.end()) {
                    return false;
                }
                map[key] = value;
                return true;
            },
            _bucket_number(key));
    }

    std::vector<KeyType> travel_keys() {
        std::vector<KeyType> keys;
        for (auto key : _maps)
            _apply([key, &keys]() { keys.push_back(key); },
                   _bucket_number(key));
        return keys;
    }

    ValueType &operator[](const KeyType &key) {
        return _apply_ref(
            [&key](HashMapType &map) -> ValueType & { return map[key]; },
            _bucket_number(key));
    }

    std::size_t size() {
        return _fold(0, [](std::size_t totalSize, const HashMapType &map) {
            return totalSize + map.size();
        });
    }

    void clear() {
        _map([](HashMapType &map) { map.clear(); });
    }

private:
    template <class ApplyFunc>
    auto &_apply_ref(ApplyFunc applyFunc, std::size_t i) {
        DCHECK(i < N) << "index " << i << " is greater than " << N;
        _locks[i].lock();
        auto &result = applyFunc(_maps[i]);
        _locks[i].unlock();
        return result;
    }

    template <class ApplyFunc>
    auto _apply(ApplyFunc applyFunc, std::size_t i) {
        DCHECK(i < N) << "index " << i << " is greater than " << N;
        _locks[i].lock();
        auto result = applyFunc(_maps[i]);
        _locks[i].unlock();
        return result;
    }

    template <class MapFunc>
    void _map(MapFunc mapFunc) {
        for (auto i = 0u; i < N; i++) {
            _locks[i].lock();
            mapFunc(_maps[i]);
            _locks[i].unlock();
        }
    }

    template <class T, class FoldFunc>
    auto _fold(const T &firstValue, FoldFunc foldFunc) {
        T finalValue = firstValue;
        for (auto i = 0u; i < N; i++) {
            _locks[i].lock();
            finalValue = foldFunc(finalValue, _maps[i]);
            _locks[i].unlock();
        }
        return finalValue;
    }

    auto _bucket_number(const KeyType &key) { return _hasher(key) % N; }

private:
    HasherType _hasher;
    HashMapType _maps[N];
    SpinLock _locks[N];
};

#endif  // DMVCCA_HASHMAP_H
