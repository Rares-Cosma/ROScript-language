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

void VM::push(VMValue v) {
    stack.push_back(v);
}

VMValue VM::pop() {
    VMValue v = stack.back();
    stack.pop_back();
    return v;
}

bool zero_check(VMValue v, vector<string> stringPool){
    if (v.type==VAL_INT && v.asInt==0) return true;
    else if (v.type==VAL_FLOAT && v.asFloat==0.0) return true;
    else if (v.type==VAL_STRING) {
        if (stringPool[v.asString]=="") return true;
    }
    return false;
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
                push(VMValue{VAL_INT,.asInt=val});
                break;
            }
            case OP_PUSH_FLOAT: {
                double val;
                memcpy(&val, &bytecode[ip], sizeof(double)); // 8 bytes
                ip += sizeof(double);
                push(VMValue{VAL_FLOAT, .asFloat = val});
                break;
            }
            case OP_PUSH_STRING: {
                int64_t val;
                memcpy(&val, &bytecode[ip], 8);
                ip += 8;
                //val == string lenght 4 bytes
                string s(reinterpret_cast<const char*>(&bytecode[ip]), val);
                ip += val;

                stringPool.push_back(s);
                int32_t idx=stringPool.size()-1;
                push(VMValue{VAL_STRING,.asString=idx});

                break;
            }
            case OP_ADD: { 
                VMValue b=pop();
                VMValue a=pop(); 
                ValueType vint=VAL_INT;
                ValueType vflt=VAL_FLOAT;
                ValueType vstr=VAL_STRING;

                if (a.type==vint && b.type==vint) {
                    push(VMValue{VAL_INT,.asInt=a.asInt+b.asInt});
                } else if (a.type==vflt && b.type==vflt) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asFloat+b.asFloat});
                } else if (a.type==vflt && b.type==vint) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asFloat+b.asInt});
                } else if (a.type==vint && b.type==vflt) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asInt+b.asFloat});
                } else if (a.type==vstr && b.type==vstr) {
                    stringPool.push_back(stringPool[a.asString]+stringPool[b.asString]);
                    int32_t idx=stringPool.size()-1;
                    push(VMValue{VAL_STRING,.asString=idx});
                }

                break; 
            }
            case OP_SUB: { 
                VMValue b=pop();
                VMValue a=pop(); 
                ValueType vint=VAL_INT;
                ValueType vflt=VAL_FLOAT;
                ValueType vstr=VAL_STRING;

                if (a.type==vint && b.type==vint) {
                    push(VMValue{VAL_INT,.asInt=a.asInt-b.asInt});
                } else if (a.type==vflt && b.type==vflt) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asFloat-b.asFloat});
                } else if (a.type==vflt && b.type==vint) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asFloat-b.asInt});
                } else if (a.type==vint && b.type==vflt) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asInt-b.asFloat});
                } else if (a.type==vstr && b.type==vstr) {
                    // throw error
                }

                break; 
            }
            case OP_MUL: { 
                VMValue b=pop();
                VMValue a=pop(); 
                ValueType vint=VAL_INT;
                ValueType vflt=VAL_FLOAT;
                ValueType vstr=VAL_STRING;

                if (a.type==vint && b.type==vint) {
                    push(VMValue{VAL_INT,.asInt=a.asInt*b.asInt});
                } else if (a.type==vflt && b.type==vflt) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asFloat*b.asFloat});
                } else if (a.type==vflt && b.type==vint) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asFloat*b.asInt});
                } else if (a.type==vint && b.type==vflt) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asInt*b.asFloat});
                }
                break; 
            }
            case OP_DIV: { 
                VMValue b=pop();
                VMValue a=pop(); 
                ValueType vint=VAL_INT;
                ValueType vflt=VAL_FLOAT;
                ValueType vstr=VAL_STRING;

                if (a.type==vint && b.type==vint) {
                    push(VMValue{VAL_INT,.asInt=a.asInt/b.asInt});
                } else if (a.type==vflt && b.type==vflt) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asFloat/b.asFloat});
                } else if (a.type==vflt && b.type==vint) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asFloat/b.asInt});
                } else if (a.type==vint && b.type==vflt) {
                    push(VMValue{VAL_FLOAT,.asFloat=a.asInt/b.asFloat});
                }
                break; 
            }
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

                VMValue value=pop();
                if (zero_check(value,stringPool)) ip = addr;
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
                    it->second(stack,argc,stringPool);
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
