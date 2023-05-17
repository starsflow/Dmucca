//
// Created by lxc on 5/10/23.
//

#ifndef DMVCCA_YSCHEMA_H
#define DMVCCA_YSCHEMA_H

#include "Global.h"
#include "Row.h"
#include "ClassOf.h"
#include "YContext.h"

static constexpr auto __BASE_COUNTER__ = __COUNTER__ + 1;
static constexpr auto YCSB_FIELD_SIZE = 10;

#define YCSB_KEY_FIELDS(x, y) x(std::size_t, Y_KEY)
#define YCSB_KEY_NAME YKey
#define YCSB_VALUE_NAME YValue
#define YCSB_VALUE_FIELDS(x, y)                                                  \
    x(std::string, Y_F01)                                       \
        y(std::string, Y_F02)                                   \
            y(std::string, Y_F03)                               \
                y(std::string, Y_F04)                           \
                    y(std::string, Y_F05)                       \
                        y(std::string, Y_F06)                   \
                            y(std::string, Y_F07)               \
                                y(std::string, Y_F08)           \
                                    y(std::string, Y_F09)       \
                                        y(std::string, Y_F10)

STRUCT_KEY(YCSB_KEY_NAME, YCSB_KEY_FIELDS);

STRUCT_VALUE(YCSB_VALUE_NAME, YCSB_VALUE_FIELDS);
#endif //DMVCCA_YSCHEMA_H