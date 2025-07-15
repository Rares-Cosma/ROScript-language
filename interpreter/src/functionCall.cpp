#include "interpreter.h"
#include "functionCall.h"
using namespace std;

Value functionCallEval(const vector<ASTNode*>& funcArgs, const vector<Value>& args, const vector<ASTNode*>& block) {
    if (funcArgs.size() != args.size()) {
        throw runtime_error("Function call argument count mismatch");
    }

    unordered_map<string, Value> localVariables;

    for (size_t i = 0; i < funcArgs.size(); ++i) {
        if (auto varDecl = dynamic_cast<VariableDeclaration*>(funcArgs[i])) {
            variables[varDecl->name] = args[i];
        }
    }

    return interpret(block, false, true, false);
}