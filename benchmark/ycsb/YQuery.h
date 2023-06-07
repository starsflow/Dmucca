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
    std::size_t Y_KEY[YContext::keysPerTransaction];
    bool UPDATE[YContext::keysPerTransaction];

public:
    YQuery() {
        int readOnly = Random::uniform_dist(1, 100);
        std::unordered_set<std::size_t> keys;

        for (auto i = 0u; i < YContext::keysPerTransaction; i++) {
            // read or write
            if (readOnly <= YContext::readOnlyTransaction) {
                this->UPDATE[i] = false;
            } else {
                int readOrWrite = Random::uniform_dist(1, 100);
                if (readOrWrite <= YContext::readWriteRatio) {
                    this->UPDATE[i] = false;
                } else {
                    this->UPDATE[i] = true;
                }
            }

            std::size_t key;
            do {
                if (YContext::isUniform) {
                    key = Random::uniform_dist(0, static_cast<int>(YContext::keysPerTransaction) - 1);
                } else {
                    key = Zipf::globalZipf().value(Random::next_double());
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
