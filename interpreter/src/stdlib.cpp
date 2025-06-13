#include "variables.h"
#include <functional>
#include <cmath>
#include <iostream>

using BuiltinFunc = function<Value(const vector<Value>&)>;

inline unordered_map<string, BuiltinFunc> stdlib = {
    {"int", [](const vector<Value>& args) {
        if (args.size() != 1) {
            throw "int function expects a single argument";
        }
        if (holds_alternative<int>(args[0])) {
            return args[0]; // already a Value holding int
        } else if (holds_alternative<float>(args[0])) {
            return Value{static_cast<int>(round(get<float>(args[0])))};
        } else if (holds_alternative<string>(args[0])) {
            return Value{stoi(get<string>(args[0]))};
        } else if (holds_alternative<bool>(args[0])) {
            return Value{get<bool>(args[0]) ? 1 : 0};
        } else {
            throw "int function cannot convert the provided type";
        }
    }},
    {"float", [](const vector<Value>& args) {
        if (args.size() != 1) {
            throw "float function expects a single argument";
        }
        if (holds_alternative<float>(args[0])) {
            return args[0]; // already a Value holding float
        } else if (holds_alternative<int>(args[0])) {
            return Value{static_cast<float>(get<int>(args[0]))};
        } else if (holds_alternative<string>(args[0])) {
            return Value{stof(get<string>(args[0]))};
        } else if (holds_alternative<bool>(args[0])) {
            return Value{get<bool>(args[0]) ? 1.0f : 0.0f};
        } else {
            throw "float function cannot convert the provided type";
        }
    }},
    {"bool", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "bool function expects a single argument";

        if (holds_alternative<bool>(args[0]))
            return args[0];
        if (holds_alternative<int>(args[0]))
            return Value{get<int>(args[0]) != 0};
        if (holds_alternative<float>(args[0]))
            return Value{get<float>(args[0]) != 0.0f};
        if (holds_alternative<string>(args[0]))
            return Value{!get<string>(args[0]).empty()};

        throw "bool function cannot convert the provided type";
    }},
    {"string", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "string function expects a single argument";

        if (holds_alternative<string>(args[0]))
            return args[0];
        if (holds_alternative<int>(args[0]))
            return Value{to_string(get<int>(args[0]))};
        if (holds_alternative<float>(args[0]))
            return Value{to_string(get<float>(args[0]))};
        if (holds_alternative<bool>(args[0]))
            return Value{get<bool>(args[0]) ? "true" : "false"};

        throw "string function cannot convert the provided type";
    }},
    {"lungime", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "len function expects a single argument";

        if (holds_alternative<string>(args[0]))
            return Value{static_cast<int>(get<string>(args[0]).length())};
        else throw "len function expects a string argument";

        throw "len function cannot convert the provided type";
    }},
    {"tip", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "type function expects a single argument";

        if (holds_alternative<int>(args[0]))
            return Value{"int"};
        if (holds_alternative<float>(args[0]))
            return Value{"float"};
        if (holds_alternative<string>(args[0]))
            return Value{"string"};
        if (holds_alternative<bool>(args[0]))
            return Value{"bool"};

        throw "type function cannot determine the type of the provided value";
    }},
    {"citeste", [](const vector<Value>& args) {
        if (args.size() > 1) {
            throw "citeste function expects a single string argument";
        }
        if (args.size()==0){
            string input;
            getline(cin, input);
            return Value{input}; // return the input as a string
        }
        string prompt = get<string>(args[0]);
        cout << prompt;
        string input;
        getline(cin, input);
        return Value{input}; // return the input as a string
    }},
    {"sqrt", [](const vector<Value>& args) {
        if (args.size() != 1) {
            throw "sqrt function expects a single argument";
        }
        if (holds_alternative<int>(args[0])) {
            return Value{sqrt(static_cast<float>(get<int>(args[0])))};
        } else if (holds_alternative<float>(args[0])) {
            return Value{sqrt(get<float>(args[0]))};
        } else {
            throw "sqrt function expects an int or float argument";
        }
    }},
    {"afiseaza", [](const vector<Value>& args) {
        for (auto& arg : args) {
            if (holds_alternative<int>(arg)) {
                cout << get<int>(arg);
            } else if (holds_alternative<float>(arg)) {
                cout << get<float>(arg);
            } else if (holds_alternative<string>(arg)) {
                cout << get<string>(arg);
            } else if (holds_alternative<bool>(arg)) {
                cout << (get<bool>(arg) ? "true" : "false");
            } else {
                throw "afiseaza function cannot handle the provided type";
            }
        }
        return Value{0}; // indicate success
    }}
};