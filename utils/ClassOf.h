/*
 * @Author: lxc
 * @Date: 05/10/2023
 */
 
#pragma once

#include "Global.h"
#include <cstddef>

template <class T> class ClassOf {
public:
    static constexpr std::size_t size() { return sizeof(T); }
};

