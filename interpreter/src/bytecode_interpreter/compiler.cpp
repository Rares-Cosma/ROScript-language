#include "compiler.h"

enum OpCode : uint8_t {
    OP_PUSH_INT,
    OP_PUSH_FLOAT,
    OP_PUSH_STRING,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_PRINT,
    OP_LOAD_VAR,
    OP_STORE_VAR,
    OP_JMP,
    OP_JMP_IF_FALSE,
    OP_CALL,
    OP_RET,
    OP_HALT,
};

vector<uint8_t> bc; // our main bytecode stack
unordered_map<string, uint8_t> variableID; // storing variable positions
size_t currentVariableIndex=0;

void emit(uint8_t b) {
    bc.push_back(b);
}

void emitInt(int32_t val) {
    uint8_t* p = reinterpret_cast<uint8_t*>(&val);
    for (int i = 0; i < 4; ++i)
        bc.push_back(p[i]);
}

void emitFloat(float val) {
    uint8_t* p = reinterpret_cast<uint8_t*>(&val);
    for (int i = 0; i < 4; ++i)
        bc.push_back(p[i]);
}

void emitString(const std::string& s) {
    emitInt((int32_t)s.size());
    bc.insert(bc.end(), s.begin(), s.end());
}

void emitEval(Expr* expr) {
    if (auto intLit = dynamic_cast<IntLiteral*>(expr)) {
        emit(OP_PUSH_INT);
        emitInt(intLit->value);
    } else if (auto bin = dynamic_cast<BinaryExpr*>(expr)) {
        emitEval(bin->left);
        emitEval(bin->right);

        if (bin->op == "+") emit(OP_ADD);
        else if (bin->op == "-") emit(OP_SUB);
        else if (bin->op == "*") emit(OP_MUL);
        else if (bin->op == "/") emit(OP_DIV);
    }
}

void compile(vector<ASTNode*> tree){
    for (size_t i=0; i<tree.size(); i++) {
        if (auto vD=dynamic_cast<VariableDeclaration*>(tree[i])){
            string type=vD->type;
            Expr* expr=vD->value;
            emitEval(expr);
            emit(OP_STORE_VAR);
            emitInt(currentVariableIndex++);
        }
    }
    cout<<bc.size();
}