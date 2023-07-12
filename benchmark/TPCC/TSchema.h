/*
 * @Author: lxc
 * @Date: 07/03/2023
 */

#pragma once

#include "ClassOf.h"
#include "Hash.h"
#include "Global.h"
#include "Row.h"

#undef NAMESPACE_FIELDS
#define NAMESPACE_FIELDS(x) x(aria) x(tpcc)

#define WAREHOUSE_KEY_FIELDS(x, y) x(int32_t, W_ID)
#define WAREHOUSE_VALUE_FIELDS(x, y)                                           \
  x(std::string, W_NAME) y(std::string, W_STREET_1)                    \
      y(std::string, W_STREET_2) y(std::string, W_CITY)                \
          y(std::string, W_STATE) y(std::string, W_ZIP) y(float, W_TAX)  \
              y(float, W_YTD)

STRUCT_ROW(WKey, WValue, WAREHOUSE_KEY_FIELDS, WAREHOUSE_VALUE_FIELDS)

#define DISTRICT_KEY_FIELDS(x, y) x(int32_t, D_W_ID) y(int32_t, D_ID)
#define DISTRICT_VALUE_FIELDS(x, y)                                            \
  x(std::string, D_NAME) y(std::string, D_STREET_1)                    \
      y(std::string, D_STREET_2) y(std::string, D_CITY)                \
          y(std::string, D_STATE) y(std::string, D_ZIP) y(float, D_TAX)  \
              y(float, D_YTD) y(int32_t, D_NEXT_O_ID)

STRUCT_ROW(DKey, DValue, DISTRICT_KEY_FIELDS, DISTRICT_VALUE_FIELDS)

#define CUSTOMER_KEY_FIELDS(x, y)                                              \
  x(int32_t, C_W_ID) y(int32_t, C_D_ID) y(int32_t, C_ID)
#define CUSTOMER_VALUE_FIELDS(x, y)                                            \
  x(std::string, C_FIRST) y(std::string, C_MIDDLE)                      \
      y(std::string, C_LAST) y(std::string, C_STREET_1)                \
          y(std::string, C_STREET_2) y(std::string, C_CITY)            \
              y(std::string, C_STATE) y(std::string, C_ZIP)              \
                  y(std::string, C_PHONE) y(uint64_t, C_SINCE)             \
                      y(std::string, C_CREDIT) y(float, C_CREDIT_LIM)       \
                          y(float, C_DISCOUNT) y(float, C_BALANCE)             \
                              y(float, C_YTD_PAYMENT)                          \
                                  y(int32_t, C_PAYMENT_CNT)                    \
                                      y(int32_t, C_DELIVERY_CNT)               \
                                          y(std::string, C_DATA)

STRUCT_ROW(CKey, CValue, CUSTOMER_KEY_FIELDS, CUSTOMER_VALUE_FIELDS)

#define CUSTOMER_NAME_IDX_KEY_FIELDS(x, y)                                     \
  x(int32_t, C_W_ID) y(int32_t, C_D_ID) y(std::string, C_LAST)
#define CUSTOMER_NAME_IDX_VALUE_FIELDS(x, y) x(int32_t, C_ID)

STRUCT_ROW(CNIKey, CNIValue, CUSTOMER_NAME_IDX_KEY_FIELDS, CUSTOMER_NAME_IDX_VALUE_FIELDS)

#define HISTORY_KEY_FIELDS(x, y)                                               \
  x(int32_t, H_W_ID) y(int32_t, H_D_ID) y(int32_t, H_C_W_ID)                   \
      y(int32_t, H_C_D_ID) y(int32_t, H_C_ID) y(uint64_t, H_DATE)
#define HISTORY_VALUE_FIELDS(x, y) x(float, H_AMOUNT) y(std::string, H_DATA)

STRUCT_ROW(HKey, HValue, HISTORY_KEY_FIELDS, HISTORY_VALUE_FIELDS)

#define NEW_ORDER_KEY_FIELDS(x, y)                                             \
  x(int32_t, NO_W_ID) y(int32_t, NO_D_ID) y(int32_t, NO_O_ID)
#define NEW_ORDER_VALUE_FIELDS(x, y) x(int32_t, NO_DUMMY)

STRUCT_ROW(NOKey, NOValue, NEW_ORDER_KEY_FIELDS, NEW_ORDER_VALUE_FIELDS)

#define ORDER_KEY_FIELDS(x, y)                                                 \
  x(int32_t, O_W_ID) y(int32_t, O_D_ID) y(int32_t, O_ID)
#define ORDER_VALUE_FIELDS(x, y)                                               \
  x(float, O_C_ID) y(uint64_t, O_ENTRY_D) y(int32_t, O_CARRIER_ID)             \
      y(int8_t, O_OL_CNT) y(bool, O_ALL_LOCAL)

STRUCT_ROW(OKey, OValue, ORDER_KEY_FIELDS, ORDER_VALUE_FIELDS)

#define ORDER_LINE_KEY_FIELDS(x, y)                                            \
  x(int32_t, OL_W_ID) y(int32_t, OL_D_ID) y(int32_t, OL_O_ID)                  \
      y(int8_t, OL_NUMBER)
#define ORDER_LINE_VALUE_FIELDS(x, y)                                          \
  x(int32_t, OL_I_ID) y(int32_t, OL_SUPPLY_W_ID) y(uint64_t, OL_DELIVERY_D)    \
      y(int8_t, OL_QUANTITY) y(float, OL_AMOUNT)                               \
          y(std::string, OL_DIST_INFO)

STRUCT_ROW(OLKey, OLValue, ORDER_LINE_KEY_FIELDS, ORDER_LINE_VALUE_FIELDS)

#define ITEM_KEY_FIELDS(x, y) x(int32_t, I_ID)
#define ITEM_VALUE_FIELDS(x, y)                                                \
  x(int32_t, I_IM_ID) y(std::string, I_NAME) y(float, I_PRICE)             \
      y(std::string, I_DATA)

STRUCT_ROW(IKey, IValue, ITEM_KEY_FIELDS, ITEM_VALUE_FIELDS)

#define STOCK_KEY_FIELDS(x, y) x(int32_t, S_W_ID) y(int32_t, S_I_ID)
#define STOCK_VALUE_FIELDS(x, y)                                               \
  x(int16_t, S_QUANTITY) y(std::string, S_DIST_01)                         \
      y(std::string, S_DIST_02) y(std::string, S_DIST_03)              \
          y(std::string, S_DIST_04) y(std::string, S_DIST_05)          \
              y(std::string, S_DIST_06) y(std::string, S_DIST_07)      \
                  y(std::string, S_DIST_08) y(std::string, S_DIST_09)  \
                      y(std::string, S_DIST_10) y(float, S_YTD)            \
                          y(int32_t, S_ORDER_CNT) y(int32_t, S_REMOTE_CNT)     \
                              y(std::string, S_DATA)

STRUCT_ROW(SKey, SValue, STOCK_KEY_FIELDS, STOCK_VALUE_FIELDS)