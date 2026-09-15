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

    {"intreg", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) {
            throw runtime_error("int function expects a single argument");
        }

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_INT) {
            stack.push_back(val);
        } else if (val.type == VAL_FLOAT) {
            stack.push_back(VMValue{VAL_INT, .asInt = static_cast<int32_t>(round(val.asFloat))});
        } else if (val.type == VAL_STRING) {
            stack.push_back(VMValue{VAL_INT, .asInt = stoi(stringPool[val.asString])});
        } else if (val.type == VAL_BOOL) {
            stack.push_back(VMValue{VAL_INT, .asInt = val.asBool ? 1 : 0});
        } else {
            throw runtime_error("int function cannot convert the provided type");
        }
    }},

    {"real", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) {
            throw runtime_error("float function expects a single argument");
        }

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_FLOAT) {
            stack.push_back(val);
        } else if (val.type == VAL_INT) {
            stack.push_back(VMValue{VAL_FLOAT, .asFloat = static_cast<double>(val.asInt)});
        } else if (val.type == VAL_STRING) {
            stack.push_back(VMValue{VAL_FLOAT, .asFloat = stof(stringPool[val.asString])});
        } else if (val.type == VAL_BOOL) {
            stack.push_back(VMValue{VAL_FLOAT, .asFloat = val.asBool ? 1.0 : 0.0});
        } else {
            throw runtime_error("float function cannot convert the provided type");
        }
    }},

    {"oprire", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {
        exit(0);
    }},

    {"lista", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) {
            throw runtime_error("list function expects a single argument");
        }

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_LIST) {
            stack.push_back(val);
        } else if (val.type == VAL_INT || val.type == VAL_FLOAT || val.type == VAL_BOOL) {

            auto vec = vector<VMValue>();
            vec.push_back(val);

            uint32_t listIndex = static_cast<uint32_t>(listPool.size());
            listPool.push_back(vec);

            stack.push_back(VMValue{VAL_LIST, .asList = listIndex});

        } else if (val.type == VAL_STRING) {

            auto vec = vector<VMValue>();

            for (char c : stringPool[val.asString]) {

                uint32_t stringIndex = static_cast<uint32_t>(stringPool.size());
                stringPool.push_back(string(1, c));

                vec.push_back(VMValue{VAL_STRING, .asString = stringIndex});
            }

            uint32_t listIndex = static_cast<uint32_t>(listPool.size());
            listPool.push_back(vec);

            stack.push_back(VMValue{VAL_LIST, .asList = listIndex});

        } else {
            throw runtime_error("list function cannot convert the provided type");
        }
    }},

    {"logic", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) {
            throw runtime_error("bool function expects a single argument");
        }

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_BOOL) {
            stack.push_back(val);
        } else if (val.type == VAL_INT) {
            stack.push_back(VMValue{VAL_BOOL, .asBool = val.asInt != 0});
        } else if (val.type == VAL_FLOAT) {
            stack.push_back(VMValue{VAL_BOOL, .asBool = val.asFloat != 0.0});
        } else if (val.type == VAL_STRING) {
            stack.push_back(VMValue{VAL_BOOL, .asBool = !stringPool[val.asString].empty()});
        } else {
            throw runtime_error("bool function cannot convert the provided type");
        }
    }},

    {"sirc", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1)
            throw runtime_error("string function expects a single argument");

        VMValue val = stack.back();
        stack.pop_back();

        string result;

        if (val.type == VAL_STRING) {
            result = stringPool[val.asString];
        } else if (val.type == VAL_INT) {
            result = to_string(val.asInt);
        } else if (val.type == VAL_FLOAT) {
            result = to_string(val.asFloat);
        } else if (val.type == VAL_BOOL) {
            result = val.asBool ? "adevarat" : "fals";
        } else if (val.type == VAL_LIST) {

            auto vec = listPool[val.asList];

            for (const auto& el : vec) {
                if (el.type == VAL_STRING) {
                    result += stringPool[el.asString];
                } else if (el.type == VAL_INT) {
                    result += to_string(el.asInt);
                } else if (el.type == VAL_FLOAT) {
                    result += to_string(el.asFloat);
                } else {
                    throw runtime_error("string function expects a list of strings or numbers");
                }
            }

        } else {
            throw runtime_error("string function cannot convert the provided type");
        }

        uint32_t stringIndex = static_cast<uint32_t>(stringPool.size());
        stringPool.push_back(result);

        stack.push_back(VMValue{VAL_STRING, .asString = stringIndex});
    }},

    {"lungime", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1)
            throw runtime_error("len function expects a single argument");

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_STRING) {

            stack.push_back(VMValue{
                VAL_INT,
                .asInt = static_cast<int32_t>(stringPool[val.asString].length())
            });

        } else if (val.type == VAL_LIST) {

            stack.push_back(VMValue{
                VAL_INT,
                .asInt = static_cast<int32_t>(listPool[val.asList].size())
            });

        } else {
            throw runtime_error("len function expects a string argument");
        }
    }},

    {"tip", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1)
            throw runtime_error("type function expects a single argument");

        VMValue val = stack.back();
        stack.pop_back();

        string type;

        if (val.type == VAL_INT) {
            type = "intreg";
        } else if (val.type == VAL_FLOAT) {
            type = "real";
        } else if (val.type == VAL_STRING) {
            type = "sirc";
        } else if (val.type == VAL_BOOL) {
            type = "logic";
        } else if (val.type == VAL_LIST) {
            type = "lista";
        } else {
            type = "necunoscut";
        }

        uint32_t stringIndex = static_cast<uint32_t>(stringPool.size());
        stringPool.push_back(type);

        stack.push_back(VMValue{VAL_STRING, .asString = stringIndex});
    }},

    {"citeste", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc > 1) {
            throw runtime_error("citeste function expects a single string argument");
        }

        if (argc == 1) {

            VMValue prompt = stack.back();
            stack.pop_back();

            cout << stringPool[prompt.asString];
            cout.flush();
        }

        string input;
        getline(cin, input);

        uint32_t stringIndex = static_cast<uint32_t>(stringPool.size());
        stringPool.push_back(input);

        stack.push_back(VMValue{VAL_STRING, .asString = stringIndex});
    }},

    {"afiseaza", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        vector<VMValue> args;

        while (argc > 0){

            VMValue val = stack.back();
            stack.pop_back();

            args.push_back(val);
            argc--;
        }

        reverse(args.begin(), args.end());

        string output;

        for (auto& arg : args){
            output += vmValueToString(arg, stringPool, listPool);
        }

        cout << output << endl;
    }}

};

vector<string> VMinitBuiltinNames() {

    vector<string> builtinNames;

    for (const auto& kv : VMstdlib) {
        builtinNames.push_back(kv.first);
    }

    return builtinNames;
}
