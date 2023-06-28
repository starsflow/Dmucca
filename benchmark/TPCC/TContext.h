/*
 * @Author: lxc
 * @Date: 06/28/2023
 */
#pragma once

#include "Global.h"
enum class TPCCWorkloadType { NEW_ORDER_ONLY, PAYMENT_ONLY, MIXED };

class TContext {
    TPCCWorkloadType workloadType = TPCCWorkloadType::NEW_ORDER_ONLY;

    int n_district = 10;
    int newOrderCrossPartitionProbability = 10;  // out of 100
    int paymentCrossPartitionProbability = 15;   // out of 100

    bool write_to_w_ytd = true;
    bool payment_look_up = false;
};