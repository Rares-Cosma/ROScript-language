#pragma once
#include "variables.h"
#include <functional>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

using BuiltinFunc = function<Value(const vector<Value>&)>;
extern unordered_map<string, BuiltinFunc> stdmatematica;
vector<string> initBuiltinNames();