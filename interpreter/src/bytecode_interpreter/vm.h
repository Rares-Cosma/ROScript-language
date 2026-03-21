#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <chrono>
#include <algorithm>
#include "opCode.h"
#include "VMstdlib.h"

using namespace std;

vector<uint8_t> loadBytecode(const string &filename);

struct CallFrame {
    int32_t returnIP;
    size_t baseScope;
};

struct Scope {
    vector<VMValue> locals;
};


struct VM {
    vector<VMValue> stack;
    vector<Scope> scopeStack; // global stack of active scopes
    vector<string> stringPool;
    vector<uint8_t> bytecode;
    vector<CallFrame> callStack;
    unordered_map<int32_t,int32_t> functionPos; // fID -> bytecode address
    size_t ip = 0;

    void push(VMValue v);
    VMValue pop();
    VMValue& resolveVar(int32_t idx);
    void run();
};