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
using namespace std;

void EPCompile(vector<ASTNode*> tree, string fn);