#pragma once
#include <unordered_map>
#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <memory>
using namespace std;

struct RecursiveValue; // forward declare

using Value = std::variant<
    int,
    float,
    std::string,
    bool,
    std::shared_ptr<std::vector<RecursiveValue>>
>;

struct RecursiveValue : Value {
    using Value::Value; // inherit constructors

    RecursiveValue(const Value& v) : Value(v) {}
    RecursiveValue(Value&& v) : Value(std::move(v)) {}
};

inline string variant_to_stringx(const Value& v) {
    if (holds_alternative<int>(v)) return to_string(get<int>(v));
    if (holds_alternative<float>(v)) return to_string(get<float>(v));
    if (holds_alternative<string>(v)) return get<string>(v);
	if (holds_alternative<bool>(v)) return to_string(get<bool>(v));
    return "NDT"; // handle cases where the type is unsupported
}

class Environment {
public:
    std::unordered_map<std::string, Value> variables;
    std::unordered_map<std::string, string> types;
    Environment* parent;

    Environment(Environment* parent = nullptr)
        : parent(parent) {}

    bool contains(const string& name) {
        return variables.count(name) > 0;
    }

    Value get(const string& name) {
        if (contains(name))
            return variables[name];
        else if (parent)
            return parent->get(name);
        else
            throw "Variable not found: " + name;
    }

    void set(const string& name, const Value& value) {
        if (contains(name))
            variables[name] = value;
        else if (parent)
            parent->set(name, value);
        else
            throw "Undefined variable: " + name;
    }

    string getType(const string& name) {
        if (types.count(name) > 0)
            return types[name];
        else if (parent)
            return parent->getType(name);
        else
            throw "Type not found for variable: " + name;
    }

    void setType(const string& name, const string& type) {
        if (types.count(name) > 0)
            types[name] = type;
        else if (parent)
            parent->setType(name, type);
        else
            throw "Undefined variable: " + name;
    }

    void define(const std::string& name, const Value& value) {
        variables[name] = value;
    }

    void defineType(const string& name, const string& type) {
        types[name] = type;
    }
};


extern Environment* currentEnv;