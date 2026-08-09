#pragma once
#include <functional>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../variables.h"

using BuiltinFunc = function<Value(const vector<Value>&)>;
extern unordered_map<string, BuiltinFunc> stdlib;
vector<string> initBuiltinNamesStdlib();