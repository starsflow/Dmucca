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
    int readWriteRatio = 50;            // out of 100
    int readOnlyTransaction = 5;       //  out of 100
    bool isUniform = false;
    double zipfFactor = 0.5;

    std::size_t keysPerTransaction = 10;
    std::size_t keysPerTable = 10000;
};