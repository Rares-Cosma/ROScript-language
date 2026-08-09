#include "VMstdlib.h"

static string vmValueToString(const VMValue& v, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {
    switch (v.type) {
        case VAL_INT: return to_string(v.asInt);
        case VAL_FLOAT: return to_string(v.asFloat);
        case VAL_BOOL: return v.asBool ? "adevarat" : "fals";
        case VAL_STRING: return stringPool[v.asString];
        case VAL_LIST: {
            string out = "[";
            auto& lst = listPool[v.asList];
            for (size_t i = 0; i < lst.size(); ++i) {
                out += vmValueToString(lst[i], stringPool, listPool);
                if (i + 1 < lst.size()) out += ", ";
            }
            out += "]";
            return out;
        }
        default: return "?";
    }
}

unordered_map<string, VMBuiltinFunc> VMstdlib = {
    {"afiseaza", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {
        vector<VMValue> args;
        while (argc>0){
            VMValue val = stack.back(); 
            stack.pop_back();
            args.push_back(val);
            argc--;
        }
        reverse(args.begin(),args.end());
        for (auto& i : args){
            cout << vmValueToString(i, stringPool, listPool);
        }
    }},
    {"oprire", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {
        exit(0);
    }},
    {"lungime", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {
        if (argc != 1) throw std::runtime_error("lungime expects exactly 1 argument");

        VMValue val = stack.back();
        stack.pop_back();

        int32_t len;
        if (val.type == VAL_LIST) {
            len = static_cast<int32_t>(listPool[val.asList].size());
        } else if (val.type == VAL_STRING) {
            len = static_cast<int32_t>(stringPool[val.asString].size());
        } else {
            throw std::runtime_error("lungime expects a list or string");
        }

        stack.push_back(VMValue{VAL_INT, .asInt = len});
    }},
    {"radp", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {
        if (argc != 1) throw std::runtime_error("radp expects exactly 1 argument");

        VMValue val = stack.back();
        stack.pop_back();

        double num;
        if (val.type == VAL_INT) num = static_cast<double>(val.asInt);
        else if (val.type == VAL_FLOAT) num = val.asFloat;
        else throw std::runtime_error("radp expects a number");

        if (num < 0) throw std::runtime_error("radp: cannot take square root of a negative number");

        stack.push_back(VMValue{VAL_FLOAT, .asFloat = sqrt(num)});
    }},
    {"abs", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {
        if (argc != 1) throw std::runtime_error("abs expects exactly 1 argument");
        VMValue val = stack.back();
        stack.pop_back();
        if (val.type == VAL_INT) stack.push_back(VMValue{VAL_INT, .asInt = std::abs(val.asInt)});
        else if (val.type == VAL_FLOAT) stack.push_back(VMValue{VAL_FLOAT, .asFloat = std::fabs(val.asFloat)});
        else throw std::runtime_error("abs expects a number");
    }},
    {"adauga", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {
        if (argc != 2) throw std::runtime_error("adauga expects exactly 2 arguments (list, value)");

        // pushed as adauga(list, value) -> value is on top, list is below
        VMValue value = stack.back(); stack.pop_back();
        VMValue listVal = stack.back(); stack.pop_back();

        if (listVal.type != VAL_LIST) throw std::runtime_error("adauga expects a list as the first argument");

        // mutate the shared listPool entry directly -> reference semantics,
        // so any other VMValue pointing at the same listPool index sees the update too
        listPool[listVal.asList].push_back(value);

        // push the list back so `adauga` can be chained/used as an expression
        stack.push_back(listVal);
    }},
};

vector<string> VMinitBuiltinNames() {
    vector<string> builtinNames;
    for (const auto& kv : VMstdlib) {
        builtinNames.push_back(kv.first);
    }
    return builtinNames;
}