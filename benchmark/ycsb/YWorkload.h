//
// Created by lxc on 5/11/23.
//

#ifndef DMVCCA_YWORKLOAD_H
#define DMVCCA_YWORKLOAD_H

#include "Global.h"
#include "HashMap.h"
#include "YContext.h"
#include "YRandom.h"
#include "Zipf.h"

template <std::size_t N>
class YWorkload {
public:
    std::size_t Y_KEY[N];
    bool UPDATE[N];

    YWorkload(YContext &context, YRandom &random) {
        int readOnly = random.uniform_dist(1, 100);
        std::unordered_set<std::size_t> keys;

        for (auto i = 0u; i < N; i++) {
            // read or write
            if (readOnly <= context.readOnlyTransaction) {
                this->UPDATE[i] = false;
            } else {
                int readOrWrite = random.uniform_dist(1, 100);
                if (readOrWrite <= context.readWriteRatio) {
                    this->UPDATE[i] = false;
                } else {
                    this->UPDATE[i] = true;
                }
            }

            std::size_t key;
            do {
                if (context.isUniform) {
                    key = random.uniform_dist(0, static_cast<int>(N) - 1);
                } else {
                    key = Zipf::globalZipf().value(random.next_double());
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

#endif  // DMVCCA_YWORKLOAD_H
