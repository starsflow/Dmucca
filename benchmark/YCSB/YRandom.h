/*
 * @Author: lxc
 * @Date: 05/10/2023
 */

#pragma once

#include "Global.h"
#include "Random.h"

class YRandom : public Random {
public:
    std::string generate_random_str(std::size_t length) {
        std::string characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        auto characters_len = characters.size();
        std::string result;
        for (auto i = 0u; i < length; i++) {
            int k = uniform_dist(0, characters_len - 1);
            result += characters[k];
        }
        return result;
    }
};