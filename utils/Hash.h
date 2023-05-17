//
// Created by lxc on 5/17/23.
//

#ifndef DMVCCA_HASH_H
#define DMVCCA_HASH_H

#include "Global.h"

class Hash {
private:
    template<typename T>
    inline static std::size_t hash_combine(const T &v1, const T &v2) {
        return v2 ^ (v1 + 0x9e3779b9 + (v2 << 6) + (v2 >> 2));
    }
public:
    template<typename T>
    inline static std::size_t hash(const T &v) {
        return std::hash<T>()(v);
    }

    template<typename T, typename... Rest>
    inline static std::size_t hash(const T &v, Rest... rest) {
        std::hash <T> h;
        return hash_combine(h(v), hash(rest...));
    }
};

#endif //DMVCCA_HASH_H
