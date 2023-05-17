//
// Created by lxc on 5/10/23.
//

#ifndef DMVCCA_YTABLE_H
#define DMVCCA_YTABLE_H

#include "Global.h"
#include "Table.h"
#include "YRandom.h"
#include "YContext.h"
#include "YSchema.h"
#include "Database.h"

class YTable : public Table<YKey, YValue> {
private:
    // only a table is needed in ycsb
    Database* _db;
    Table<YKey, YValue>* _table;
    YRandom _random;

public:
    void initialize_table(){
        _table = _db->create_table<YKey, YValue>();
        for(std::size_t i = 0; i < YContext::keysPerTable; i++) {
            YKey key;
            key.Y_KEY = i;
            YValue value;
            value.Y_F01.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            value.Y_F02.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            value.Y_F03.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            value.Y_F04.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            value.Y_F05.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            value.Y_F06.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            value.Y_F07.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            value.Y_F08.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            value.Y_F09.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            value.Y_F10.assign(_random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE));
            
            _table->insert(key, value);
        }
    }
};

#endif //DMVCCA_YTABLE_H
