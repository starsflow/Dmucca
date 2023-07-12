/*
 * @Author: lxc
 * @Date: 07/04/2023
 */

#pragma once

#include "Database.h"
#include "Global.h"
#include "TContext.h"
#include "TRandom.h"
#include "TSchema.h"
#include "Table.h"
#include "Time.h"

class Tables {
private:
    Database* _db;
    TContext _context;

public:
    Tables(Database* db) : _db(db), _context(){};

    void initialize_warehouse_table() {
        TRandom _random = TRandom(Time::now());
        ITable* w_table = static_cast<ITable*>(_db->create_table<WKey, WValue>());
        for (std::size_t i = 0; i < _context.w; i++) {
            WKey key;
            WValue value;
            key.W_ID = i;
            value.W_NAME.assign(_random.n_string(1, 10));
            value.W_STREET_1.assign(_random.n_string(10, 20));
            value.W_STREET_2.assign(_random.n_string(10, 20));
            value.W_CITY.assign(_random.n_string(10, 20));
            value.W_STATE.assign(_random.n_string(2, 2));
            value.W_ZIP.assign(_random.n_string(9, 9));
            value.W_TAX = static_cast<float>(_random.uniform_dist(0, 2000)) / 10000;
            value.W_YTD = _context.w_ytd;
            w_table->insert(&key, 0, &value);
        }
    }

    void initialize_district_table() {
        TRandom _random = TRandom(Time::now());
        ITable* d_table = static_cast<ITable*>(_db->create_table<DKey, DValue>());
        for (std::size_t i = 0; i < _context.w; i++) {
            for (std::size_t j = 0; j < 10; j++) {
                DKey key;
                DValue value;
                key.D_W_ID = i;
                key.D_ID = j;
                value.D_NAME.assign(_random.n_string(1, 10));
                value.D_STREET_1.assign(_random.n_string(10, 20));
                value.D_STREET_2.assign(_random.n_string(10, 20));
                value.D_CITY.assign(_random.n_string(10, 20));
                value.D_STATE.assign(_random.n_string(2, 2));
                value.D_ZIP.assign(_random.n_string(9, 9));
                value.D_TAX = static_cast<float>(_random.uniform_dist(0, 2000)) / 10000;
                value.D_YTD = _context.w_ytd;
                value.D_NEXT_O_ID = _context.w_ytd + 1;
                d_table->insert(&key, 0, &value);
            }
        }
    }

    void initialize_customer_table() {
        TRandom _random = TRandom(Time::now());
        ITable* c_table = static_cast<ITable*>(_db->create_table<CKey, CValue>());
        for (std::size_t i = 0; i < _context.w; i++) {
            for (std::size_t j = 0; j < 10; j++) {
                for (std::size_t k = 0; k < 3000; k++) {
                    CKey key;
                    CValue value;
                    key.C_W_ID = i;
                    key.C_D_ID = j;
                    key.C_ID = k;
                    value.C_MIDDLE.assign("OE");
                    value.C_FIRST.assign(_random.n_string(8, 16));
                    value.C_STREET_1.assign(_random.n_string(10, 20));
                    value.C_STREET_2.assign(_random.n_string(10, 20));
                    value.C_CITY.assign(_random.n_string(10, 20));
                    value.C_STATE.assign(_random.n_string(2, 2));
                    value.C_ZIP.assign(_random.n_string(9, 9));
                    value.C_PHONE.assign(_random.n_string(16, 16));
                    value.C_SINCE = Time::now();
                    value.C_CREDIT_LIM = 50000;
                    value.C_DISCOUNT = static_cast<float>(_random.uniform_dist(0, 5000)) / 10000;
                    value.C_BALANCE = -10;
                    value.C_YTD_PAYMENT = 10;
                    value.C_PAYMENT_CNT = 1;
                    value.C_DELIVERY_CNT = 1;
                    value.C_DATA.assign(_random.n_string(300, 500));

                    int last_name = k <= 1000 ? j - 1 : _random.non_uniform_distribution(255, 0, 999);
                    value.C_LAST.assign(_random.rand_last_name(last_name));

                    int x = _random.uniform_dist(1, 10);
                    x == 1 ? value.C_CREDIT.assign("BC") : value.C_CREDIT.assign("GC");
                    c_table->insert(&key, 0, &value);
                }
            }
        }
    }

    void initialize_item_table() {
        TRandom _random = TRandom(Time::now());
        ITable* c_table = static_cast<ITable*>(_db->create_table<IKey, IValue>());
        for (std::size_t i = 0; i < _context.i_cnt; i++) {
            IKey key;
            IValue value;
            key.I_ID = i;
            value.I_IM_ID = i;
            value.I_NAME.assign(_random.n_string(10, 24));
            value.I_PRICE = static_cast<float>(_random.uniform_dist(10, 100));
            value.I_DATA.assign(_random.n_string(20, 50));
            c_table->insert(&key, 0, &value);
        }
    }

    void initialize_stock_table() {
        TRandom _random = TRandom(Time::now());
        ITable* s_table = static_cast<ITable*>(_db->create_table<SKey, SValue>());
        for (std::size_t i = 0; i < _context.w; i++) {
            for (std::size_t j = 0; j < _context.i_cnt; j++) {
                SKey key;
                SValue value;
                key.S_W_ID = i;
                key.S_I_ID = j;
                value.S_QUANTITY = _random.uniform_dist(100, 1000);
                value.S_DIST_01.assign(_random.a_string(24, 24));
                value.S_DIST_02.assign(_random.a_string(24, 24));
                value.S_DIST_03.assign(_random.a_string(24, 24));
                value.S_DIST_04.assign(_random.a_string(24, 24));
                value.S_DIST_05.assign(_random.a_string(24, 24));
                value.S_DIST_06.assign(_random.a_string(24, 24));
                value.S_DIST_07.assign(_random.a_string(24, 24));
                value.S_DIST_08.assign(_random.a_string(24, 24));
                value.S_DIST_09.assign(_random.a_string(24, 24));
                value.S_DIST_10.assign(_random.a_string(24, 24));
                value.S_YTD = 0;
                value.S_ORDER_CNT = 0;
                value.S_REMOTE_CNT = 0;
                s_table->insert(&key, 0, &value);
            }
        }
    }
};