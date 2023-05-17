//
// Created by lxc on 5/10/23.
//

#ifndef DMVCCA_CLASSOF_H
#define DMVCCA_CLASSOF_H

#include "Global.h"
#include <cstddef>

template <class T> class ClassOf {
public:
    static constexpr std::size_t size() { return sizeof(T); }
};

#endif //DMVCCA_CLASSOF_H
