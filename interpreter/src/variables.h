#pragma once
#include <unordered_map>
#include <string>
#include <variant>
using namespace std;
using Value = variant<int, float, string, bool>;

extern std::unordered_map<std::string, Value> variables;