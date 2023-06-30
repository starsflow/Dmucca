/*
 * @Author: lxc
 * @Date: 05/11/2023
 */

#pragma once

#include "Global.h"

class Zipf {
public:
    void init(int n, double theta) {
        hasInit = true;
        _n = n;
        _theta = theta;
        _alpha = 1.0 / (1.0 - _theta);
        _z_etan = zeta(_n);
        _eta = (1.0 - std::pow(2.0 / _n, 1.0 - _theta)) / (1.0 - zeta(2) / _z_etan);
    }

    int value(double u) {
        CHECK(hasInit);

        double uz = u * _z_etan;
        int v;
        if (uz < 1) {
            v = 0;
        } else if (uz < 1 + std::pow(0.5, _theta)) {
            v = 1;
        } else {
            v = static_cast<int>(_n * std::pow(_eta * u - _eta + 1, _alpha));
        }
        DCHECK(v >= 0 && v < _n);
        return v;
    }

    static Zipf &globalZipf() {
        static Zipf z;
        return z;
    }

private:
    double zeta(int n) {
        DCHECK(hasInit);
        double sum = 0;
        for (auto i = 1; i <= n; i++) {
            sum += std::pow(1.0 / i, _theta);
        }
        return sum;
    }

    bool hasInit = false;
    int _n;
    double _theta;
    double _alpha;
    double _z_etan;
    double _eta;
};

