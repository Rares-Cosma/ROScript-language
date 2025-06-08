#include "variables.h"
#include <functional>

using BuiltinFunc = function<Value(const vector<Value>&)>;

inline unordered_map<string, BuiltinFunc> stdlib = {
    {"int", [](const vector<Value>& args) {
        if (args.size() != 1) {
            throw "int function expects a single argument";
        }
        if (holds_alternative<int>(args[0])) {
            return args[0]; // already a Value holding int
        } else if (holds_alternative<float>(args[0])) {
            return Value{static_cast<int>(get<float>(args[0]))};
        } else if (holds_alternative<string>(args[0])) {
            return Value{stoi(get<string>(args[0]))};
        } else if (holds_alternative<bool>(args[0])) {
            return Value{get<bool>(args[0]) ? 1 : 0};
        } else {
            throw "int function cannot convert the provided type";
        }
    }}
};