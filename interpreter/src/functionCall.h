#pragma once
#include <vector>
#include <iostream>
#include <variant>
#include <string>

class ASTNode;
using Value = std::variant<int, float, std::string, bool>;

Value functionCallEval(const std::vector<ASTNode*>& funcArgs, const std::vector<Value>& args, const std::vector<ASTNode*>& block);