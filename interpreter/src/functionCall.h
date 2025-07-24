#pragma once
#include <vector>
#include <iostream>
#include <variant>
#include <string>
#include "variables.h"

class ASTNode;


Value functionCallEval(const std::vector<ASTNode*>& funcArgs, const std::vector<Value>& args, const std::vector<ASTNode*>& block);