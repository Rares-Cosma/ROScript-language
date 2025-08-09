#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include "opCode.h"
#include "VMstdlib.h"

using namespace std;

vector<uint8_t> loadBytecode(const string &filename);

struct VM {
    vector<VMValue> stack;
    vector<VMValue> variables;
    vector<string> stringPool;
    vector<uint8_t> bytecode;
    size_t ip = 0;

    void push(VMValue v);
    VMValue pop();
    void run();
};