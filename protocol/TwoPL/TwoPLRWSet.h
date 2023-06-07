/*
 * @Author: lxc
 * @Date: 05/22/2023
 */
#pragma once

#include "Global.h"

struct ReadItem {
    void* key;
    //storage the read results
    void* dest;
};

struct WriteItem {
    void* key;
    //storage the value to be written to db
    void* sour;
};