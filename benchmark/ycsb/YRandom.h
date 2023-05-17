//
// Created by lxc on 5/10/23.
//

#ifndef DMVCCA_YRANDOM_H
#define DMVCCA_YRANDOM_H

#include "Global.h"
#include "Random.h"

class YRandom : public Random {
public:
    using Random::Random;

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

#endif //DMVCCA_YRANDOM_H
