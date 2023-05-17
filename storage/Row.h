//
// Created by lxc on 5/10/23.
//

#ifndef DMVCCA_ROW_H
#define DMVCCA_ROW_H

#include "Global.h"
#include "Hash.h"

#define APPLY_X_AND_Y(x, y) x(y, y)

#define STRUCT_PARAM_FIRST_X(type, name) type name

#define STRUCT_PARAM_REST_X(type, name) , type name

#define STRUCT_INITLIST_FIRST_X(type, name) name(name)

#define STRUCT_INITLIST_REST_X(type, name) , name(name)

#define STRUCT_HASH_FIRST_X(type, name) k.name

#define STRUCT_HASH_REST_X(type, name) , k.name

#define STRUCT_LAYOUT_X(type, name) type name;

#define STRUCT_EQ_X(type, name)                                              \
  if (this->name != other.name)                                              \
    return false;

#define STRUCT_FIELDPOS_X(type, name) name##_field,

// the main macro
#define STRUCT_KEY(name, keyfields)                                                 \
    class name{                                                                \
    public:                                                                  \
        keyfields(STRUCT_LAYOUT_X, STRUCT_LAYOUT_X)                           \
        name(){}                                                                      \
        name(keyfields(STRUCT_PARAM_FIRST_X, STRUCT_PARAM_REST_X)) :           \
            keyfields(STRUCT_INITLIST_FIRST_X, STRUCT_INITLIST_REST_X()){}    \
                                                                              \
        bool operator==(name other) const {                        \
                APPLY_X_AND_Y(keyfields, STRUCT_EQ_X)                          \
                return true;                                                   \
            }                                                                  \
        bool operator!=(name other) const {                        \
                return !operator==(other);                                     \
            }                                                                  \
                                                                              \
        bool is_key_equals(name other){                                        \
            keyfields(STRUCT_EQ_X, STRUCT_EQ_X)                               \
            return true;                                                      \
        }                                                                     \
        static std::size_t hash_key(name k) {                  \
            return Hash::hash(keyfields(STRUCT_HASH_FIRST_X, STRUCT_HASH_REST_X));                                                                     \
        }                                                                      \
    };

#define STRUCT_VALUE(name, valuefields)                                             \
    class name{                                                              \
    public:                                                                  \
        valuefields(STRUCT_LAYOUT_X, STRUCT_LAYOUT_X)                         \
        name(){}                                                                      \
        name(valuefields(STRUCT_PARAM_FIRST_X, STRUCT_PARAM_REST_X)) :         \
            valuefields(STRUCT_INITLIST_FIRST_X, STRUCT_INITLIST_REST_X){}  \
                                                                              \
        bool is_value_equals(name other){                                        \
            valuefields(STRUCT_EQ_X, STRUCT_EQ_X)                               \
            return true;                                                      \
        }                                                                     \
    };

//#define STRUCT_ROW(keyfields, valuefields)                                    \
//    class Row{                                                                \
//    private:                                                                  \
//        keyfields(STRUCT_LAYOUT_X, STRUCT_LAYOUT_X)                           \
//        valuefields(STRUCT_LAYOUT_X, STRUCT_LAYOUT_X)                         \
//    public:                                                                   \
//        Row(keyfields(STRUCT_PARAM_FIRST_X, STRUCT_PARAM_REST_X),             \
//            valuefields(STRUCT_PARAM_FIRST_X, STRUCT_PARAM_REST_X)) :         \
//            keyfields(STRUCT_INITLIST_FIRST_X, STRUCT_INITLIST_REST_X()),     \
//            valuefields(STRUCT_INITLIST_FIRST_X, STRUCT_INITLIST_REST_X){}    \
//                                                                              \
//        bool is_key_equals(Row other){                                        \
//            keyfields(STRUCT_EQ_X, STRUCT_EQ_X)                               \
//            return true;                                                      \
//        }                                                                     \
//                                                                              \
//        bool is_value_equals(Row other){                                      \
//            valuefields(STRUCT_EQ_X, STRUCT_EQ_X)                             \
//            return true;                                                      \
//        }                                                                     \
//                                                                              \
//    };
#endif //DMVCCA_ROW_H
