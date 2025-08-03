#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cstdint>
using namespace std;

using VMBuiltinFunc = void(*)(vector<int32_t>& stack, uint32_t argc);
extern unordered_map<string, VMBuiltinFunc> VMstdlib;
vector<string> VMinitBuiltinNames();