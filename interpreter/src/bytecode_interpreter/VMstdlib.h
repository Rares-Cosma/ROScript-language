#pragma once
#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <string>
#include <iostream>
using namespace std;

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

using VMBuiltinFunc = void(*)(vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool);
extern unordered_map<string, VMBuiltinFunc> VMstdlib;
vector<string> VMinitBuiltinNames();