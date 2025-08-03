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
    vector<int32_t> stack;
    vector<int32_t> variables;
    vector<uint8_t> bytecode;
    size_t ip = 0;

    void push(int32_t v);
    int32_t pop();
    void run();
};