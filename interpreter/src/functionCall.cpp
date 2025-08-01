#include "interpreter.h"
#include "variables.h"
#include "functionCall.h"
using namespace std;

Value functionCallEval(const vector<ASTNode*>& funcArgs, const vector<Value>& args, const vector<ASTNode*>& block) 
{
    if (funcArgs.size() != args.size()) {
        throw runtime_error("Function call argument count mismatch");
    }

    Environment* previous = currentEnv;
    Environment* functionScope = new Environment(previous);
    currentEnv = functionScope;

    for (size_t i = 0; i < funcArgs.size(); ++i) {
        if (auto varDecl = dynamic_cast<VariableDeclaration*>(funcArgs[i])) {
            functionScope->define(varDecl->name, args[i]);
            functionScope->defineType(varDecl->name, varDecl->type);
        }
    }

    Value res = interpret(block, false, true, false);

    currentEnv = previous;
    delete functionScope;

    return res;
}
