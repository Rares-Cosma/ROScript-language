#include "compiler.h"
#include <unordered_map>

vector<uint8_t> bc; // our main bytecode stack
vector<string> builtIns;
size_t currentVariableIndex=0;
uint8_t functionIDCounter=0;
unordered_map<string,int> funcID;
vector<uint8_t> localCounterStack; // one entry per active function call being compiled

void emit(uint8_t b) {
    bc.push_back(b);
}

void emitInt(int32_t val) {
    uint8_t* p = reinterpret_cast<uint8_t*>(&val);
    for (int i = 0; i < 4; ++i)
        bc.push_back(p[i]);
}

void emitFloat(double val) {
    uint8_t* p = reinterpret_cast<uint8_t*>(&val);
    for (int i = 0; i < 8; ++i)
        bc.push_back(p[i]);
}

void emitString(const string& s) {
    uint64_t len = s.size();
    uint8_t* p = reinterpret_cast<uint8_t*>(&len); 
    for (int i = 0; i < 8; ++i) {
        bc.push_back(p[i]);
    }
    bc.insert(bc.end(), s.begin(), s.end());
}

void emitBool(const bool b) {
    if (b) {
        uint8_t v=1;
        bc.push_back(v);
    } else {
        uint8_t v=0;
        bc.push_back(v);
    }
}

uint8_t getVariableID(const string& name);
uint8_t getSTDFunctionID(string name);
uint8_t getFunctionID(string name);

void emitEval(Expr* expr) {
    if (auto une = dynamic_cast<UnaryExpr*>(expr)){
        emitEval(une->expr);
    
        if (une->op == "-") {
            emit(OP_NEG);   // unary minus
        } else if (une->op == "!") {
            emit(OP_NOT);   // logical NOT
        } else {
            throw runtime_error("Unknown unary operator: " + une->op);
        }
        return;
    }

    if (auto intLit = dynamic_cast<IntLiteral*>(expr)) {
        emit(OP_PUSH_INT);
        emitInt(intLit->value);
    } else if (auto floatLit = dynamic_cast<FloatLiteral*>(expr)) {
        emit(OP_PUSH_FLOAT);
        emitFloat(floatLit->value);
    } else if (auto boolLit = dynamic_cast<BoolLiteral*>(expr)) {
        emit(OP_PUSH_BOOL);
        emitBool(boolLit->value);
    } else if (auto strLit = dynamic_cast<StringLiteral*>(expr)) {
        emit(OP_PUSH_STRING);
        emitString(strLit->value);
    } else if (auto listLit = dynamic_cast<ListLiteral*>(expr)) {
        for (auto* element : listLit->args) {
            emitEval(element);
        }
        emit(OP_LIST_CREATE);
        emitInt(listLit->args.size());
    } else if (auto listIdx = dynamic_cast<ListIndex*>(expr)) {
        emit(OP_LOAD_VAR);
        emitInt(getVariableID(listIdx->name));

        for (auto* idxExpr : listIdx->index) {
            emitEval(idxExpr); 
            emit(OP_LIST_GET); 
        }

    } else if (auto rf = dynamic_cast<Refrence*>(expr)) {
        emit(OP_LOAD_VAR);
        emitInt(getVariableID(rf->name));
    } else if (auto fC = dynamic_cast<FunctionCall*>(expr)) {
        if (stdlib.find(fC->name) != stdlib.end()) {
            for (auto* arg : fC->args) {
                emitEval(arg);
            }
            emit(OP_CALL_DEFAULT);
            emitInt(getSTDFunctionID(fC->name));
            emitInt(fC->args.size());
        } else {
            for (auto* arg : fC->args) {
                emitEval(arg);
            }
            emit(OP_CALL);
            emitInt(getFunctionID(fC->name));
            emitInt(fC->args.size());
        }
    // add the list ones
    } else if (auto bin = dynamic_cast<BinaryExpr*>(expr)) {
        emitEval(bin->left);
        emitEval(bin->right);

        if (bin->op == "+") emit(OP_ADD);
        else if (bin->op == "-") emit(OP_SUB);
        else if (bin->op == "*") emit(OP_MUL);
        else if (bin->op == "/") emit(OP_DIV);
        else if (bin->op == "%") emit(OP_MOD);
        else if (bin->op == ">") emit(OP_GT);
        else if (bin->op == "<") emit(OP_LT);
        else if (bin->op == ">=") emit(OP_GE);
        else if (bin->op == "<=") emit(OP_LE);
        else if (bin->op == "==") emit(OP_EE);
        else if (bin->op == "!=") emit(OP_NE);
        else if (bin->op == "&&") emit(OP_AND);
        else if (bin->op == "||") emit(OP_OR);
    }
}

void saveBytecodeToFile(const string& filename) {
    ofstream out(filename, ios::binary);
    if (!out) {
        cerr << "Failed to open file for writing: " << filename << endl;
        return;
    }
    out.write(reinterpret_cast<const char*>(bc.data()), bc.size());
    out.close();
}

void visualizeBytecode(const vector<uint8_t>& bytecode) {
    size_t i = 0;
    bool store_var = false;
    while (i < bytecode.size()) {
        uint8_t op = bytecode[i++];
        cout << setw(4) << (i - 1) << ": ";

        switch (op) {
            case OP_PUSH_INT: {
                int32_t val;
                memcpy(&val, &bytecode[i], 4);
                i += 4;
                cout << "PUSH_INT " << val << "\n";
                break;
            }
            case OP_PUSH_FLOAT: {
                double fval;
                memcpy(&fval, &bytecode[i], 8);
                i += 8;
                cout << "PUSH_FLOAT " << fval << "\n";
                break;
            }
            case OP_PUSH_STRING: {
                int64_t len;
                memcpy(&len, &bytecode[i], 8);
                i += 8;
                string s((const char*)&bytecode[i], len);
                i += len;
                cout << "PUSH_STRING \"" << s << "\"\n";
                break;
            }
            case OP_PUSH_BOOL: {
                bool b = bytecode[i++] != 0;
                cout << "PUSH_BOOL " << (b ? "adevarat" : "fals") << "\n";
                break;
            }
            case OP_ADD: cout << "ADD\n"; break;
            case OP_SUB: cout << "SUB\n"; break;
            case OP_MUL: cout << "MUL\n"; break;
            case OP_DIV: cout << "DIV\n"; break;
            case OP_MOD: cout << "MOD\n"; break;
            case OP_EE: cout << "EQUALS-EQUALS\n"; break;
            case OP_NE: cout << "NOT-EQUALS\n"; break;
            case OP_GT: cout << "GREATER-THAN\n"; break;
            case OP_LT: cout << "LESS-THAN\n"; break;
            case OP_GE: cout << "GREATER-OR-EQUALS\n"; break;
            case OP_LE: cout << "LESS-OR-EQUALS\n"; break;
            case OP_AND: cout << "AND\n"; break;
            case OP_OR: cout << "OR\n"; break;
            case OP_NEG: cout << "NEG\n"; break;
            case OP_NOT: cout << "NOT\n"; break;

            case OP_TYPE_INT: cout << "TYPE_INT\n"; break;
            case OP_TYPE_FLOAT: cout << "TYPE_FLOAT\n"; break;
            case OP_TYPE_STRING: cout << "TYPE_STRING\n"; break;
            case OP_TYPE_BOOL: cout << "TYPE_BOOL\n"; break;
            case OP_TYPE_LIST: cout << "TYPE_LIST\n"; break;
            case OP_TYPE_NDT: cout << "TYPE_NDT\n"; break;

            case OP_LIST_CREATE: {
                int32_t count;
                memcpy(&count, &bytecode[i], 4);
                i += 4;
                cout << "LIST_CREATE with " << count << " elements\n";
                break;
            }
            case OP_LIST_GET: cout << "LIST_GET\n"; break;
            case OP_LIST_SET: cout << "LIST_SET\n"; break;

            case OP_LOAD_VAR: {
                int32_t idx;
                memcpy(&idx, &bytecode[i], 4);
                i += 4;
                cout << "LOAD_VAR " << idx << "\n";
                break;
            }
            case OP_STORE_VAR: {
                int32_t idx;
                memcpy(&idx, &bytecode[i], 4);
                i += 4;
                cout << "STORE_VAR " << idx << "\n";
                break;
            }

            case OP_JMP: {
                int32_t addr;
                memcpy(&addr, &bytecode[i], 4);
                i += 4;
                cout << "JMP " << addr << "\n";
                break;
            }
            case OP_JMP_IF_FALSE: {
                int32_t addr;
                memcpy(&addr, &bytecode[i], 4);
                i += 4;
                cout << "JMP_IF_FALSE " << addr << "\n";
                break;
            }
            case OP_CALL: {
                int32_t fn;
                memcpy(&fn, &bytecode[i], 4);
                i += 4;
                int32_t argc;
                memcpy(&argc, &bytecode[i], 4);
                i += 4;
                cout << "CALL fn@" << fn << " argc "<< argc  << "\n";
                break;
            }
             case OP_CALL_DEFAULT: {
                int32_t fn;
                memcpy(&fn, &bytecode[i], 4);
                i += 4;
                int32_t argc;
                memcpy(&argc, &bytecode[i], 4);
                i += 4;
                cout << "CALL DEF fn@" << fn << " argc "<< argc  << "\n";
                break;
            }
            case OP_RET: cout << "RET\n"; break;
            case OP_HALT: cout << "HALT\n"; break;

            case OP_FDECL: {
                int32_t pos;
                memcpy(&pos, &bytecode[i], 4);
                i += 4;
                int32_t fID;
                memcpy(&fID, &bytecode[i], 4);
                i += 4;
                cout << "FDECL fn@" << fID << " at "<< pos  << "\n";
                break;
            }

            default:
                cout << "UNKNOWN(" << (int)op << ")\n";
                break;
        }
    }
}

struct Scope {
    unordered_map<string, uint8_t> locals; 
};
enum VarType {
    VAR_INT,
    VAR_FLOAT,
    VAR_STRING,
    VAR_BOOL,
    VAR_LIST,
    VAR_NDT
};
vector<Scope> scopeStack;
vector<vector<VarType>> variableTypes; // for each scope, the types of variables declared

bool variableExists(const string& name) {
    for (int i = scopeStack.size() - 1; i >= 0; --i) {
        if (scopeStack[i].locals.find(name) != scopeStack[i].locals.end()) {
            return true;
        }
    }
    return false;
}

uint8_t getVariableID(const string& name) {
    for (int i = scopeStack.size() - 1; i >= 0; i--) {
        auto it = scopeStack[i].locals.find(name);
        if (it != scopeStack[i].locals.end()) return it->second;
    }
    throw runtime_error("Variable not found: " + name);
}
VarType getVariableType(const string& name) {
    for (int i = scopeStack.size() - 1; i >= 0; --i) {
        auto it = scopeStack[i].locals.find(name);
        if (it != scopeStack[i].locals.end()) {
            return variableTypes[i][it->second];
        }
    }
    throw runtime_error("Variable type not found: " + name);
}


void declareVariable(const string& name, uint8_t id) {
    scopeStack.back().locals[name] = id;
}
void declareVariableType(const string& name, VarType type) {
    if (scopeStack.empty()) {
        throw runtime_error("No active scope to declare variable type");
    }
    if (variableTypes.size() <= scopeStack.size() - 1) {
        variableTypes.resize(scopeStack.size());
    }
    variableTypes[scopeStack.size() - 1].push_back(type);
}

uint8_t getSTDFunctionID(string name){
    auto it = find(builtIns.begin(), builtIns.end(), name);
    if (it == builtIns.end()) {
        throw runtime_error("Undefined builtin function: " + name);
    }
    return static_cast<uint8_t>(it - builtIns.begin());
}

uint8_t getFunctionID(string name){
    if (funcID.find(name) == funcID.end()) {
        funcID[name]=functionIDCounter;
        return functionIDCounter++;
    } else {
        return funcID[name];
    }
}

void compile(vector<ASTNode*> tree, string fn) {

    for (size_t i = 0; i < tree.size(); i++) {
        if (auto vD = dynamic_cast<VariableDeclaration*>(tree[i])) {
            Expr* expr = vD->value;
            emitEval(expr);

            VarType varType;
            if (vD->type == "INT") varType = VAR_INT;
            else if (vD->type == "FLOAT") varType = VAR_FLOAT;
            else if (vD->type == "STRING") varType = VAR_STRING;
            else if (vD->type == "BOOL") varType = VAR_BOOL;
            else if (vD->type == "NDT") varType = VAR_NDT;
            else if (vD->type == "LIST") varType = VAR_LIST;
            else throw runtime_error("Unknown variable type: " + vD->type);

            // declare variable in current scope
            uint8_t id = localCounterStack.back()++;
            declareVariable(vD->name, id);
            declareVariableType(vD->name, varType);

            // emit STORE_VAR
            emit(OP_STORE_VAR);
            emitInt(id);

            // emit type
            if (varType == VAR_INT) emit(OP_TYPE_INT);
            else if (varType == VAR_FLOAT) emit(OP_TYPE_FLOAT);
            else if (varType == VAR_STRING) emit(OP_TYPE_STRING);
            else if (varType == VAR_BOOL) emit(OP_TYPE_BOOL);
            else if (varType == VAR_LIST) emit(OP_TYPE_LIST);
            else if (varType == VAR_NDT) emit(OP_TYPE_NDT);

        } else if (auto vA = dynamic_cast<AssignStatement*>(tree[i])) {
            if (vA->index) {
                emitEval(vA->expr);

                emit(OP_LOAD_VAR);
                emitInt(getVariableID(vA->name));

                for (size_t k = 0; k < vA->indexList.size(); ++k) {
                    emitEval(vA->indexList[k]);
                    if (k + 1 < vA->indexList.size()) {
                        emit(OP_LIST_GET); 
                    } else {
                        emit(OP_LIST_SET);
                    }
                }
            } else {
                emitEval(vA->expr);
                uint8_t id = getVariableID(vA->name);
                emit(OP_STORE_VAR);
                emitInt(id);

                VarType t = getVariableType(vA->name);
                if (t == VAR_INT) emit(OP_TYPE_INT);
                else if (t == VAR_FLOAT) emit(OP_TYPE_FLOAT);
                else if (t == VAR_STRING) emit(OP_TYPE_STRING);
                else if (t == VAR_BOOL) emit(OP_TYPE_BOOL);
                else if (t == VAR_NDT) emit(OP_TYPE_NDT);
            }
        } else if (auto iS = dynamic_cast<ImportStatement*>(tree[i])) {
            continue;
        } else if (auto forS = dynamic_cast<ForStatement*>(tree[i])) {
            scopeStack.push_back(Scope{});

            compile({forS->init_block}, fn);

            size_t loopStart = bc.size();
            emitEval(forS->expr);
            emit(OP_JMP_IF_FALSE);
            size_t jumpFalsePos = bc.size();
            emitInt(0);

            compile(forS->block, fn);
            compile({forS->assign_block}, fn);

            emit(OP_JMP);
            emitInt(static_cast<int32_t>(loopStart));

            size_t loopEnd = bc.size();
            memcpy(&bc[jumpFalsePos], &loopEnd, 4);

            scopeStack.pop_back();

        } else if (auto wh = dynamic_cast<WhileStatement*>(tree[i])) {
            scopeStack.push_back(Scope{});

            size_t loopStart = bc.size();
            emitEval(wh->expr);
            emit(OP_JMP_IF_FALSE);
            size_t jumpFalsePos = bc.size();
            emitInt(0);

            compile(wh->block, fn);

            emit(OP_JMP);
            emitInt(static_cast<int32_t>(loopStart));

            size_t loopEnd = bc.size();
            memcpy(&bc[jumpFalsePos], &loopEnd, 4);

            scopeStack.pop_back();

        } else if (auto ifs = dynamic_cast<IfStatement*>(tree[i])) {
            vector<size_t> endJumps;

            // --- IF branch ---
            scopeStack.push_back(Scope{});
            emitEval(ifs->expr);
            emit(OP_JMP_IF_FALSE);
            size_t ifFalsePos = bc.size();
            emitInt(0);
            compile(ifs->block, fn);
            scopeStack.pop_back();

            emit(OP_JMP);
            endJumps.push_back(bc.size());
            emitInt(0);

            int32_t elseAddr = bc.size();
            memcpy(&bc[ifFalsePos], &elseAddr, 4);

            // --- ELSE IF branches ---
            for (auto &branch : ifs->elseIfBranches) {
                scopeStack.push_back(Scope{});
                emitEval(branch.first);
                emit(OP_JMP_IF_FALSE);
                size_t elseifFalsePos = bc.size();
                emitInt(0);
                compile(branch.second, fn);
                scopeStack.pop_back();

                emit(OP_JMP);
                endJumps.push_back(bc.size());
                emitInt(0);

                int32_t nextAddr = bc.size();
                memcpy(&bc[elseifFalsePos], &nextAddr, 4);
            }

            // --- ELSE branch ---
            if (!ifs->elseBlock.empty()) {
                scopeStack.push_back(Scope{});
                compile(ifs->elseBlock, fn);
                scopeStack.pop_back();
            }

            int32_t finalAddr = bc.size();
            for (size_t pos : endJumps) memcpy(&bc[pos], &finalAddr, 4);

        } else if (auto fC = dynamic_cast<FunctionCall*>(tree[i])) {
            for (auto *arg : fC->args) emitEval(arg);

            if (VMstdlib.find(fC->name) != VMstdlib.end()) {
                int fID = getSTDFunctionID(fC->name);
                emit(OP_CALL_DEFAULT);
                emitInt(fID);
                emitInt(fC->args.size());
            } else {
                int fID = getFunctionID(fC->name);
                emit(OP_CALL);
                emitInt(fID);
                emitInt(fC->args.size());
            }

        } else if (auto rS = dynamic_cast<ReturnStatement*>(tree[i])) {
            if (rS->expr) emitEval(rS->expr);
            emit(OP_RET);

        } else if (auto fD = dynamic_cast<FunctionDefinition*>(tree[i])) {
            emit(OP_FDECL);
            emitInt(bc.size() + 13);
            emitInt(getFunctionID(fD->name));

            emit(OP_JMP);
            size_t jumpPlaceholderPos = bc.size();
            emitInt(0);

            scopeStack.push_back(Scope{});
            localCounterStack.push_back(0);

            // --- declare arguments in function scope ---
            emit(OP_JMP);
            size_t argJumpAddrPlaceHolder = bc.size();
            emitInt(0); // placeholder
            for (auto arg : fD->args) {
                compile({arg}, fn);
            }
            int32_t argAfterFuncAddr = bc.size();
            memcpy(&bc[argJumpAddrPlaceHolder], &argAfterFuncAddr, 4);
            
            compile(fD->block, fn);
            scopeStack.pop_back();
            localCounterStack.pop_back();

            int32_t afterFuncAddr = bc.size();
            memcpy(&bc[jumpPlaceholderPos], &afterFuncAddr, 4);
        }
    }

    
}

void EPCompile(vector<ASTNode*> tree, string fn){
    builtIns=VMinitBuiltinNames();

    scopeStack.push_back(Scope{}); // push global/current scope
    localCounterStack.push_back(0);

    compile(tree,fn);
    emit(OP_HALT);
    saveBytecodeToFile(fn+"bc");
    visualizeBytecode(bc);

    scopeStack.pop_back(); // pop global/current scope
    localCounterStack.pop_back();
}