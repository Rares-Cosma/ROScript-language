#pragma once
#include "../parser.h"
#include "../variables.h"
#include "./native_libs/VMstdlib.h"
#include "./native_libs/VMstdmatematica.h"
#include "./native_libs/VMstdfisier.h"
#include "./native_libs/VMstdvector.h"
#include "opCode.h"
#include <vector>
#include <iomanip>
#include <cstring>
#include <unordered_map>
#include <algorithm>
using namespace std;

using VMBuiltinFunc = void(*)(vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool);

struct native_lib {
    string name;
    string alias;
    unordered_map<string, VMBuiltinFunc> funcs;
};

extern unordered_map<string, native_lib> nativeLibs;
extern vector<string> builtIns;

void EPCompile(vector<ASTNode*> tree, string fn);