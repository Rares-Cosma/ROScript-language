#include "interpreter.h"
#include "variables.h"
#include "functionCall.h"
using namespace std;

Value functionCallEval(const vector<ASTNode*>& funcArgs, const vector<Value>& args, const vector<ASTNode*>& block) {
    if (funcArgs.size() != args.size()) {
        throw runtime_error("Function call argument count mismatch");
    }

    for (size_t i = 0; i < funcArgs.size(); ++i) {
        if (auto varDecl = dynamic_cast<VariableDeclaration*>(funcArgs[i])) {
            currentEnv->define(varDecl->name, args[i]);
            currentEnv->defineType(varDecl->name, varDecl->type); // store type of variable
        }
    }

    return interpret(block, false, true, false);
}