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

void binaryArithmetic(VM& vm, uint8_t op) {
    VMValue b = vm.pop();
    VMValue a = vm.pop();

    auto pushInt = [&](int32_t v) { vm.push(VMValue{VAL_INT, .asInt = v}); };
    auto pushFloat = [&](double v) { vm.push(VMValue{VAL_FLOAT, .asFloat = v}); };
    auto pushString = [&](const string& v) {
        vm.stringPool.push_back(v);
        vm.push(VMValue{VAL_STRING, .asString = static_cast<int32_t>(vm.stringPool.size() - 1)});
    };
    auto pushBool = [&](bool v) { vm.push(VMValue{VAL_BOOL, .asBool = v}); };

    switch (op) {
        case OP_ADD:
            if (a.type == VAL_INT && b.type == VAL_INT) pushInt(a.asInt + b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushFloat(a.asFloat + b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushFloat(a.asFloat + b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushFloat(a.asInt + b.asFloat);
            else if (a.type == VAL_STRING && b.type == VAL_STRING) pushString(vm.stringPool[a.asString] + vm.stringPool[b.asString]);
            else throw "Invalid types for ADD";
            break;

        case OP_SUB:
            if (a.type == VAL_INT && b.type == VAL_INT) pushInt(a.asInt - b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushFloat(a.asFloat - b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushFloat(a.asFloat - b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushFloat(a.asInt - b.asFloat);
            else throw "Invalid types for SUB";
            break;

        case OP_MUL:
            if (a.type == VAL_INT && b.type == VAL_INT) pushInt(a.asInt * b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushFloat(a.asFloat * b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushFloat(a.asFloat * b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushFloat(a.asInt * b.asFloat);
            else throw "Invalid types for MUL";
            break;

        case OP_DIV:
            if (a.type == VAL_INT && b.type == VAL_INT) {
                if (b.asInt == 0) throw "Division by zero";
                pushInt(a.asInt / b.asInt);
            } else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) {
                if (b.asFloat == 0.0) throw "Division by zero";
                pushFloat(a.asFloat / b.asFloat);
            } else if (a.type == VAL_FLOAT && b.type == VAL_INT) {
                if (b.asInt == 0) throw "Division by zero";
                pushFloat(a.asFloat / b.asInt);
            } else if (a.type == VAL_INT && b.type == VAL_FLOAT) {
                if (b.asFloat == 0.0) throw "Division by zero";
                pushFloat(a.asInt / b.asFloat);
            } else throw "Invalid types for DIV";
            break;
        case OP_MOD:
            if (a.type == VAL_INT && b.type == VAL_INT) {
                if (b.asInt == 0) throw "MOD by zero";
                pushInt(a.asInt % b.asInt);
            } else throw "Invalid types for MOD";
            break;
        case OP_EE:
            if (a.type == VAL_INT && b.type == VAL_INT) pushBool(a.asInt == b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushBool(a.asFloat == b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushBool(a.asFloat == b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushBool(a.asInt == b.asFloat);
            else if (a.type == VAL_STRING && b.type == VAL_STRING)
                pushBool(vm.stringPool[a.asString] == vm.stringPool[b.asString]);
            else if (a.type == VAL_BOOL && b.type == VAL_BOOL) pushBool(a.asBool== b.asBool);
            else throw "Invalid types for EQUALS-EQUALS";
            break;
        case OP_NE:
            if (a.type == VAL_INT && b.type == VAL_INT) pushBool(a.asInt != b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushBool(a.asFloat != b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushBool(a.asFloat != b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushBool(a.asInt != b.asFloat);
            else if (a.type == VAL_STRING && b.type == VAL_STRING)
                pushBool(vm.stringPool[a.asString] != vm.stringPool[b.asString]);
            else if (a.type == VAL_BOOL && b.type == VAL_BOOL) pushBool(a.asBool != b.asBool);
            else throw "Invalid types for EQUALS-EQUALS";
            break;
        case OP_GT:
            if (a.type == VAL_INT && b.type == VAL_INT) pushBool(a.asInt > b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushBool(a.asFloat > b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushBool(a.asFloat > b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushBool(a.asInt > b.asFloat);
            else if (a.type == VAL_STRING && b.type == VAL_STRING)
                pushBool(vm.stringPool[a.asString] > vm.stringPool[b.asString]);
            else throw "Invalid types for GREATER-THAN";
            break;
        case OP_LT:
            if (a.type == VAL_INT && b.type == VAL_INT) pushBool(a.asInt < b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushBool(a.asFloat < b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushBool(a.asFloat < b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushBool(a.asInt < b.asFloat);
            else if (a.type == VAL_STRING && b.type == VAL_STRING)
                pushBool(vm.stringPool[a.asString] < vm.stringPool[b.asString]);
            else throw "Invalid types for LESS-THAN";
            break;
        case OP_GE:
            if (a.type == VAL_INT && b.type == VAL_INT) pushBool(a.asInt >= b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushBool(a.asFloat >= b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushBool(a.asFloat >= b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushBool(a.asInt >= b.asFloat);
            else throw "Invalid types for GREATER-OR-EQUALS";
            break;
        case OP_LE:
            if (a.type == VAL_INT && b.type == VAL_INT) pushBool(a.asInt <= b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushBool(a.asFloat <= b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushBool(a.asFloat <= b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushBool(a.asInt <= b.asFloat);
            else throw "Invalid types for LESS-OR-EQUALS";
            break;
        case OP_AND:
            if (a.type == VAL_BOOL && b.type == VAL_BOOL) pushBool(a.asBool && b.asBool);
            else if (a.type == VAL_INT && b.type == VAL_INT) pushBool(a.asInt && b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushBool(a.asFloat && b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushBool(a.asFloat && b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushBool(a.asInt && b.asFloat);
            else if (a.type == VAL_STRING && b.type == VAL_STRING)
                pushBool(!vm.stringPool[a.asString].empty() && !vm.stringPool[b.asString].empty());
            else throw "Invalid types for AND";
            break;
        case OP_OR:
            if (a.type == VAL_BOOL && b.type == VAL_BOOL) pushBool(a.asBool || b.asBool);
            else if (a.type == VAL_INT && b.type == VAL_INT) pushBool(a.asInt || b.asInt);
            else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) pushBool(a.asFloat || b.asFloat);
            else if (a.type == VAL_FLOAT && b.type == VAL_INT) pushBool(a.asFloat || b.asInt);
            else if (a.type == VAL_INT && b.type == VAL_FLOAT) pushBool(a.asInt || b.asFloat);
            else if (a.type == VAL_STRING && b.type == VAL_STRING)
                pushBool(!vm.stringPool[a.asString].empty() || !vm.stringPool[b.asString].empty());
            else throw "Invalid types for OR";
            break;
        case OP_NEG:
            if (a.type == VAL_INT) pushInt(-a.asInt);
            else if (a.type == VAL_FLOAT) pushFloat(-a.asFloat);
            else throw "Invalid type for NEG";
            break;
        case OP_NOT:
            if (a.type == VAL_BOOL) pushBool(!a.asBool);
            else if (a.type == VAL_INT) pushBool(a.asInt == 0);
            else if (a.type == VAL_FLOAT) pushBool(a.asFloat == 0.0);
            else if (a.type == VAL_STRING) pushBool(vm.stringPool[a.asString].empty());
            else throw "Invalid type for NOT";
            break;
        default:
            throw "Unknown arithmetic operation";
    }
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
    else if (v.type==VAL_BOOL && v.asBool==false) return true;
    else if (v.type==VAL_STRING) {
        if (stringPool[v.asString]=="") return true;
    }
    return false;
}

void VM::run() {
    cout << "[VM] Execution started.\n";
    auto start= chrono::high_resolution_clock::now();
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
            case OP_PUSH_BOOL: {
                int32_t val;
                memcpy(&val, &bytecode[ip], 1);
                ip += 1;
                push(VMValue{VAL_BOOL,.asBool=val ? true : false});
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
            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
            case OP_MOD:
            case OP_EE:
            case OP_NE:
            case OP_GT:
            case OP_LT:
            case OP_GE:
            case OP_LE:
            case OP_AND:
            case OP_OR:
            case OP_NEG:
            case OP_NOT:
                binaryArithmetic(*this, op);
                break;
            case OP_STORE_VAR: {
                int32_t idx;
                memcpy(&idx, &bytecode[ip], 4);
                ip += 4;
                if (idx >= (int)variables.size()) variables.resize(idx+1);
                if (bytecode[ip] == OP_TYPE_INT) {
                    VMValue temp = pop();
                    if (temp.type == VAL_FLOAT) {
                        variables[idx] = VMValue{VAL_INT, .asInt = static_cast<int32_t>(temp.asFloat)};
                    } else if (temp.type == VAL_BOOL) {
                        variables[idx] = VMValue{VAL_INT, .asInt = temp.asBool ? 1 : 0};
                    } else if (temp.type == VAL_STRING) {
                        variables[idx] = VMValue{VAL_INT, .asInt = stoi(stringPool[temp.asString])};
                    } else if (temp.type == VAL_INT) {
                        variables[idx] = temp;
                    } else {
                        throw std::runtime_error("Invalid type for STORE_VAR");
                    }
                    ip++;
                } else if (bytecode[ip] == OP_TYPE_FLOAT) {
                    VMValue temp = pop();
                    if (temp.type == VAL_FLOAT) {
                        variables[idx] = temp;
                    } else if (temp.type == VAL_BOOL) {
                        variables[idx] = VMValue{VAL_FLOAT, .asFloat = temp.asBool ? 1.0 : 0.0};
                    } else if (temp.type == VAL_STRING) {
                        variables[idx] = VMValue{VAL_FLOAT, .asFloat = stof(stringPool[temp.asString])};
                    } else if (temp.type == VAL_INT) {
                        variables[idx] = VMValue{VAL_FLOAT, .asFloat = static_cast<double>(temp.asInt)};
                    } else {
                        throw std::runtime_error("Invalid type for STORE_VAR");
                    }
                    ip++;
                } else if (bytecode[ip] == OP_TYPE_STRING) {
                    VMValue temp = pop();
                    if (temp.type == VAL_FLOAT) {
                        string tempStr = to_string(temp.asFloat);
                        stringPool.push_back(tempStr);
                        variables[idx] = VMValue{VAL_STRING, .asString = static_cast<int32_t>(stringPool.size() - 1)};
                    } else if (temp.type == VAL_BOOL) {
                        string tempStr = temp.asBool ? "adevarat" : "fals";
                        stringPool.push_back(tempStr);
                        variables[idx] = VMValue{VAL_STRING, .asString = static_cast<int32_t>(stringPool.size() - 1)};
                    } else if (temp.type == VAL_STRING) {
                        variables[idx] = temp;
                    } else if (temp.type == VAL_INT) {
                        string tempStr = to_string(temp.asInt);
                        stringPool.push_back(tempStr);
                        variables[idx] = VMValue{VAL_STRING, .asString = static_cast<int32_t>(stringPool.size() - 1)};
                    } else {
                        throw std::runtime_error("Invalid type for STORE_VAR");
                    }
                    ip++;
                } else if (bytecode[ip] == OP_TYPE_BOOL) {
                    VMValue temp = pop();
                    if (temp.type == VAL_FLOAT) {
                        variables[idx] = VMValue{VAL_BOOL, .asBool = temp.asFloat != 0.0};
                    } else if (temp.type == VAL_BOOL) {
                        variables[idx] = temp;
                    } else if (temp.type == VAL_STRING) {
                        variables[idx] = VMValue{VAL_BOOL, .asBool = (stringPool[temp.asString] != "")};
                    } else if (temp.type == VAL_INT) {
                        variables[idx] = VMValue{VAL_BOOL, .asBool = (temp.asInt != 0)};
                    } else {
                        throw std::runtime_error("Invalid type for STORE_VAR");
                    }
                    ip++;
                } else {
                    variables[idx] = pop();
                }
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
                break;
            default:
                cerr << "\n[VM] Unknown opcode: " << (int)op << std::endl;
                return;
        }
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "[VM] Execution finished in " << duration.count() << " ms.\n";
}
