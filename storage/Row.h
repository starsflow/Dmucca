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

#define FIRST_KEY_TO_STRING(type, name) ss << #name

#define REST_KEY_TO_STRING(type, name) << ";" << #name

#define FIRST_VALUE_TO_STRING(type, name) ss << name

#define REST_VALUE_TO_STRING(type, name) << "\t" << name

#define STRUCT_EQ_X(type, name) \
    if (this->name != other.name) return false;
   
// #define STRUCT_FIELDPOS_X(type, name) name##_field,

// the main macro
#define STRUCT_ROW(keyname, valuename, keyfields, valuefields)                \
    class keyname {                                                           \
    public:                                                                   \
        keyfields(STRUCT_LAYOUT_X, STRUCT_LAYOUT_X) keyname() {}              \
        keyname(keyfields(STRUCT_PARAM_FIRST_X, STRUCT_PARAM_REST_X))         \
            : keyfields(STRUCT_INITLIST_FIRST_X, STRUCT_INITLIST_REST_X()) {} \
                                                                              \
        bool operator==(keyname other) const {                                \
            APPLY_X_AND_Y(keyfields, STRUCT_EQ_X)                             \
            return true;                                                      \
        }                                                                     \
                                                                              \
        static std::stringstream class_to_string() {                          \
            std::stringstream ss;                                             \
            keyfields(FIRST_KEY_TO_STRING, REST_KEY_TO_STRING);               \
            return ss;                                                        \
        }                                                                     \
        std::stringstream member_to_string() const {                          \
            std::stringstream ss;                                             \
            keyfields(FIRST_VALUE_TO_STRING, REST_VALUE_TO_STRING);           \
            return ss;                                                        \
        }                                                                     \
    };                                                                        \
    class valuename {                                                         \
    public:                                                                   \
        valuefields(STRUCT_LAYOUT_X, STRUCT_LAYOUT_X) valuename() {}          \
        valuename(valuefields(STRUCT_PARAM_FIRST_X, STRUCT_PARAM_REST_X))     \
            : valuefields(STRUCT_INITLIST_FIRST_X, STRUCT_INITLIST_REST_X) {} \
        bool operator==(valuename other) const {                              \
            APPLY_X_AND_Y(valuefields, STRUCT_EQ_X)                           \
            return true;                                                      \
        }                                                                     \
        static std::stringstream class_to_string() {                          \
            std::stringstream ss;                                             \
            valuefields(FIRST_KEY_TO_STRING, REST_KEY_TO_STRING);             \
            return ss;                                                        \
        }                                                                     \
        std::stringstream member_to_string() const {                          \
            std::stringstream ss;                                             \
            valuefields(FIRST_VALUE_TO_STRING, REST_VALUE_TO_STRING);         \
            return ss;                                                        \
        }                                                                     \
    };                                                                        \
    namespace std {                                                           \
    template <>                                                               \
    struct hash<keyname> {                                                    \
        std::size_t operator()(keyname k) const {                             \
            return Hash::hash(                                                \
                keyfields(STRUCT_HASH_FIRST_X, STRUCT_HASH_REST_X));          \
        }                                                                     \
    };                                                                        \
    }
#endif  // DMVCCA_ROW_H
