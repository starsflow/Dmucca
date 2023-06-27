/*
 * @Author: lxc
 * @Date: 05/11/2023
 */
#pragma once

#include "Global.h"
#include "HashMap.h"
#include "YContext.h"
#include "YRandom.h"
#include "Zipf.h"

class YQuery {
public:
    std::size_t Y_KEY[YContext::keysPerTable];
    bool UPDATE[YContext::keysPerTransaction];

private:
    YContext _context;
    YRandom _random;

public:
    YQuery(YContext& context, YRandom random) : _context(context), _random(random) {
        int readOnly = _random.uniform_dist(1, 100);
        std::unordered_set<std::size_t> keys;
        
        for (auto i = 0u; i < YContext::keysPerTransaction; i++) {
            // read or write
            if (readOnly <= _context.readOnlyTransaction) {
                this->UPDATE[i] = false;
            } else {
                int readOrWrite = _random.uniform_dist(1, 100);
                if (readOrWrite <= _context.readWriteRatio) {
                    this->UPDATE[i] = false;
                } else {
                    this->UPDATE[i] = true;
                }
            }

            std::size_t key;
            do {
                if (_context.isUniform || (_context.skewPattern == YCSBSkewPattern::READ && this->UPDATE[i]) ||
                    (_context.skewPattern == YCSBSkewPattern::WRITE && this->UPDATE[i])) {
                    key = _random.uniform_dist(0, static_cast<int>(YContext::keysPerTable) - 1);
                } else {
                    key = Zipf::globalZipf().value(_random.next_double());
                }

                auto result = keys.find(key);
                if (result == keys.end()) {
                    this->Y_KEY[i] = key;
                    break;
                }
            } while (true);
        }
    }
};
