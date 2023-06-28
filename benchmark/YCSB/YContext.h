/*
 * @Author: lxc
 * @Date: 05/10/2023
 */

#pragma once

#include "Global.h"

enum class YCSBSkewPattern { BOTH, READ, WRITE };

class YContext {
public:
    YCSBSkewPattern skewPattern = YCSBSkewPattern::BOTH;
    int readWriteRatio = 0;            // out of 100
    int readOnlyTransaction = 0;       //  out of 100
    bool isUniform = true;

    static const std::size_t keysPerTransaction = 10;
    static const std::size_t keysPerTable = 100;
};