#pragma once
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <string>
#include <iostream>
#include "./VMcommons.h"
using namespace std;

using VMBuiltinFunc = void(*)(vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool);
extern unordered_map<string, VMBuiltinFunc> VMstdfisier;
vector<string> VMinitBuiltinNamesFisier();