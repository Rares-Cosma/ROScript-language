#pragma once
#include "../parser.h"
#include "../variables.h"
#include "VMstdlib.h"
#include "opCode.h"
#include <vector>
#include <iomanip>
#include <cstring>
#include <unordered_map>
using namespace std;

void EPCompile(vector<ASTNode*> tree, string fn);