/*
 * @Author: lxc
 * @Date: 05/11/2023
 */
#pragma once

#include "Global.h"
#include "HashMap.h"
#include "YContext.h"
#include "YRandom.h"
#include "YTable.h"
#include "Zipf.h"
#include "Time.h"

class YQuery {
public:
    std::vector<YKey> Y_KEY;
    std::vector<bool> UPDATE;
    std::size_t keys_per_transaction;
    std::size_t keys_per_table;

private:
    YContext _context;
    YRandom _random;

public:
    YQuery() : _context(), _random(Time::now()) {
        keys_per_transaction = _context.keysPerTransaction;
        keys_per_table = _context.keysPerTable;

        this->UPDATE.clear();
        this->Y_KEY.clear();
        int readOnly = _random.uniform_dist(1, 100);
        std::unordered_set<std::size_t> keys;
        Zipf::globalZipf().init(keys_per_table, _context.zipfFactor);

        for (auto i = 0u; i < keys_per_transaction; i++) {
            // read or write
            if (readOnly <= _context.readOnlyTransaction) {
                this->UPDATE.emplace_back(false);
            } else {
                int readOrWrite = _random.uniform_dist(1, 100);
                if (readOrWrite <= _context.readWriteRatio) {
                    this->UPDATE.emplace_back(false);
                } else {
                    this->UPDATE.emplace_back(true);
                }
            }

            std::size_t key;
            do {
                if (_context.isUniform || (_context.skewPattern == YCSBSkewPattern::READ && this->UPDATE[i]) ||
                    (_context.skewPattern == YCSBSkewPattern::WRITE && this->UPDATE[i])) {
                    key = _random.uniform_dist(0, keys_per_table - 1);
                } else {
                    key = Zipf::globalZipf().value(_random.next_double());
                }

                auto result = keys.find(key);
                if (result == keys.end()) {
                    this->Y_KEY.emplace_back(key);
                    keys.emplace(key);
                    break;
                }
            } while (true);
        }
    }

    YQuery reset_query() { return YQuery(); }
    
    friend std::ostream& operator<<(std::ostream& os, YQuery& query) {
        for(std::size_t i = 0; i < query.keys_per_transaction; i++) {
            if(query.UPDATE[i])
                os << "w:";
            else 
                os << "r:";
            os << query.Y_KEY[i] << ";";
        }
        return os;
    }
};
