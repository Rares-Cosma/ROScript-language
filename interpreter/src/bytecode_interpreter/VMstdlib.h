#pragma once
#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <iostream>
using namespace std;

enum ValueType { VAL_INT, VAL_STRING, VAL_FLOAT, VAL_BOOL };

struct VMValue {
    ValueType     type;
    union {
        int32_t   asInt;
        double    asFloat;
        int32_t   asString;
        bool      asBool;
    };
};

using VMBuiltinFunc = void(*)(vector<VMValue>& stack, uint32_t argc, vector<string> stringPool);
extern unordered_map<string, VMBuiltinFunc> VMstdlib;
vector<string> VMinitBuiltinNames();