/*
 * @Author: lxc
 * @Date: 05/10/2023
 */

#pragma once

#include "Global.h"
#include "Table.h"
#include "YRandom.h"
#include "YContext.h"
#include "YSchema.h"
#include "Database.h"

class YTable : public Table<YKey, YValue> {
public:
    void initialize_table(){
        YContext _context;
        YRandom _random(std::chrono::system_clock::now().time_since_epoch().count());

        for(std::size_t i = 0; i < _context.keysPerTable; i++) {
            YKey key;
            key.Y_KEY = i;
            YValue* value = new YValue();
            value->Y_F01.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            // value->Y_F02.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            // value->Y_F03.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            // value->Y_F04.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            // value->Y_F05.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            // value->Y_F06.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            // value->Y_F07.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            // value->Y_F08.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            // value->Y_F09.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            // value->Y_F10.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            this->insert(&key, 0, value);
        }
        LOG(WARNING) << "table initialized!";
    }
};

