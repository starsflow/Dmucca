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

    static const int readWriteRatio = 0;            // out of 100
    static const int readOnlyTransaction = 0;       //  out of 100
    static const std::size_t keysPerTable = 100;
    static const std::size_t keysPerTransaction = 10;
    static const bool isUniform = true;
};