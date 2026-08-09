#pragma once
#include <cstdint>

enum ValueType { VAL_INT, VAL_STRING, VAL_FLOAT, VAL_BOOL, VAL_NDT, VAL_LIST };

struct VMValue {
    ValueType     type;
    union {
        int32_t   asInt;
        double    asFloat;
        int32_t   asString;
        bool      asBool;
        int32_t   asList;
    };
};