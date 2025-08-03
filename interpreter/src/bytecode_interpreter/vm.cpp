#include "vm.h"

vector<string> VMbuiltIns;
uint8_t VMgetSTDFunctionID(string name){
    auto it = find(VMbuiltIns.begin(), VMbuiltIns.end(), name);
    if (it == VMbuiltIns.end()) {
        throw "Undefined builtin function: " + name;
    }
    return static_cast<uint8_t>(it - VMbuiltIns.begin());
}

vector<uint8_t> loadBytecode(const string &filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Could not open bytecode file: " << filename << endl;
        exit(1);
    }

    vector<uint8_t> data((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return data;
}

void VM::push(int32_t v) {
    stack.push_back(v);
}

int32_t VM::pop() {
    int32_t v = stack.back();
    stack.pop_back();
    return v;
}

void VM::run() {
    VMbuiltIns=VMinitBuiltinNames();
    while (ip < bytecode.size()) {
        uint8_t op = bytecode[ip++];
        switch (op) {
            case OP_PUSH_INT: {
                int32_t val;
                memcpy(&val, &bytecode[ip], 4);
                ip += 4;
                push(val);
                break;
            }
            case OP_PUSH_STRING: {
                int32_t val;
                memcpy(&val, &bytecode[ip], 4);
                ip += 4;
                push(val);
                break;
            }
            case OP_ADD: { int b=pop(), a=pop(); push(a+b); break; }
            case OP_SUB: { int b=pop(), a=pop(); push(a-b); break; }
            case OP_MUL: { int b=pop(), a=pop(); push(a*b); break; }
            case OP_DIV: { int b=pop(), a=pop(); push(a/b); break; }
            case OP_STORE_VAR: {
                int32_t idx;
                memcpy(&idx, &bytecode[ip], 4);
                ip += 4;
                if (idx >= (int)variables.size()) variables.resize(idx+1);
                variables[idx] = pop();
                break;
            }
            case OP_LOAD_VAR: {
                int32_t idx;
                memcpy(&idx, &bytecode[ip], 4);
                ip += 4;
                push(variables[idx]);
                break;
            }
            case OP_JMP: {
                int32_t addr;
                memcpy(&addr, &bytecode[ip], 4);
                ip = addr;
                break;
            }
            case OP_JMP_IF_FALSE: {
                int32_t addr;
                memcpy(&addr, &bytecode[ip], 4);  
                ip += 4;

                int32_t value=pop();
                if (value == 0) ip = addr;
                break;
            }
            case OP_CALL_DEFAULT: {
                int32_t fID;
                memcpy(&fID, &bytecode[ip], 4);  
                ip += 4;

                int32_t argc;
                memcpy(&argc, &bytecode[ip], 4);  
                ip+=4;
                
                auto fname = VMbuiltIns[fID];
                auto it = VMstdlib.find(fname);
                if (it != VMstdlib.end()) {
                    it->second(stack,argc);
                } else {
                    throw std::runtime_error("Unknown built-in function: " + fname);
                }
                break;
            }
            case OP_HALT:
                cout << "[VM] Execution finished.\n";
                return;
            default:
                cerr << "[VM] Unknown opcode: " << (int)op << std::endl;
                return;
        }
    }
    
}
