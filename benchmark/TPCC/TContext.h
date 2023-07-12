/*
 * @Author: lxc
 * @Date: 06/28/2023
 */
#pragma once

#include "Global.h"
enum class TPCCWorkloadType { NEW_ORDER_ONLY, PAYMENT_ONLY, MIXED };

class TContext {
public:
    TPCCWorkloadType workloadType = TPCCWorkloadType::NEW_ORDER_ONLY;

    int w = 3;
    int w_ytd = 30000;
    int o_id_cnt = 100000;
    int i_cnt = 100000;

    bool write_to_w_ytd = true;
    bool payment_look_up = false;
};