#include "interpreter.h"
#include <unordered_map>
#include <chrono>
#include <random>

inline string generateRandomSentinel() {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string s(32, '0'); // 32-char random string
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    for (auto& c : s) c = alphanum[dis(gen)];
    return "RETURN_" + s;
}

const string RETURN_STANDARD = generateRandomSentinel();

using namespace chrono;

Environment* currentEnv = new Environment();
Environment* globalEnv = new Environment();

string type(Value inp){
    if (holds_alternative<int>(inp)) return "int";
    if (holds_alternative<float>(inp)) return "float";
    if (holds_alternative<string>(inp)) return "string";
    if (holds_alternative<bool>(inp)) return "bool";
    return "unknown";
}

Expr* simplify(Expr* expr) {
    if (auto ref = dynamic_cast<Refrence*>(expr)) {
        string refName = get<string>(ref->eval());

        if (currentEnv->contains(refName)) {
            Value varValue = currentEnv->get(refName);

            if (holds_alternative<int>(varValue))
                return new IntLiteral(get<int>(varValue));
            if (holds_alternative<float>(varValue))
                return new FloatLiteral(get<float>(varValue));
            if (holds_alternative<string>(varValue))
                return new StringLiteral(get<string>(varValue));
        } else {
            cout << "Variable not found: " << refName << endl;
        }
    }

    else if (auto binExpr = dynamic_cast<BinaryExpr*>(expr)) {
        binExpr->left = simplify(binExpr->left);
        binExpr->right = simplify(binExpr->right);
        return binExpr;
    }
    return expr;
}

bool condition_to_bool(const Value& conditionValue) {
    if (holds_alternative<bool>(conditionValue)) {
        return get<bool>(conditionValue);
    } else if (holds_alternative<int>(conditionValue)) {
        return get<int>(conditionValue) != 0;
    } else if (holds_alternative<float>(conditionValue)) {
        return get<float>(conditionValue) != 0.0f;
    }
    return false; // default case
}

void print_ast(const std::vector<ASTNode*>& AST, int indent = 0) {
    for (const auto& node : AST) {
        node->get(indent);
    }
    cout << string(indent, ' ') << "End of AST" << endl;
}

unordered_map<string, microseconds> node_times;
unordered_map<string, int> node_counts;

Value interpret(std::vector<ASTNode*> AST, bool fprint_ast, bool profiler, bool print_pdata) {
    if (fprint_ast){
        cout << "AST:" << endl;
        print_ast(AST);
    }
    //cout<<AST.size()<<" AST nodes found."<<endl;

    auto full_interpretation_start = high_resolution_clock::now();

    for (ASTNode* node : AST) {
        if (auto varDecl = dynamic_cast<VariableDeclaration*>(node)) {
            auto start = high_resolution_clock::now();
            //varDecl->value = simplify(varDecl->value);
            Value value = varDecl->value->eval();
            if (varDecl->type == "NDT") {
                currentEnv->define(varDecl->name, value);
            } else if (varDecl->type == "INT") {
                if (holds_alternative<float>(value)) {
                    currentEnv->define(varDecl->name,static_cast<int>(get<float>(value)));
                } else if (holds_alternative<string>(value)) {
                    currentEnv->define(varDecl->name,stoi(get<string>(value)));
                } else if (holds_alternative<int>(value)) {
                    currentEnv->define(varDecl->name,get<int>(value));
                } else {
                    cout<<"No known conversion for variable: " << varDecl->name << endl;
                }
            } else if (varDecl->type == "FLOAT") {
                if (holds_alternative<int>(value)) {
                    currentEnv->define(varDecl->name,static_cast<float>(get<int>(value)));
                } else if (holds_alternative<string>(value)) {
                    currentEnv->define(varDecl->name,stof(get<string>(value)));
                } else if (holds_alternative<float>(value)) {
                    currentEnv->define(varDecl->name,get<float>(value));
                } else {
                    cout << "No known conversion for variable: " << varDecl->name << endl;
                }
            } else if (varDecl->type == "STRING") {
                if (holds_alternative<int>(value)) {
                    currentEnv->define(varDecl->name,to_string(get<int>(value)));
                } else if (holds_alternative<float>(value)) {
                    currentEnv->define(varDecl->name,to_string(get<float>(value)));
                } else if (holds_alternative<string>(value)) {
                    currentEnv->define(varDecl->name,get<string>(value));
                } else if (holds_alternative<bool>(value)) {
                    currentEnv->define(varDecl->name,get<bool>(value) ? "adevarat" : "fals");
                } else {
                    cout << "No known conversion for variable: " << varDecl->name << endl;
                }
            } else if (varDecl->type == "BOOL") {
                currentEnv->define(varDecl->name,condition_to_bool(value));
            }
            currentEnv->defineType(varDecl->name,varDecl->type); // store type of variable
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start);
                node_times["VariableDeclaration"] += duration;
                node_counts["VariableDeclaration"]++;
            }
        } else if (auto print = dynamic_cast<PrintStatement*>(node)) {
            auto start = high_resolution_clock::now();
            //print->expr = simplify(print->expr);
            cout<<variant_to_string(print->expr->eval());
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start);
                node_times["PrintStatement"] += duration;
                node_counts["PrintStatement"]++;
            }
        } else if (auto ret = dynamic_cast<ReturnStatement*>(node)) {
            auto start = high_resolution_clock::now();
            //ret->expr = simplify(ret->expr);
            Value returnValue = ret->expr->eval();
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start);
                node_times["ReturnStatement"] += duration;
                node_counts["ReturnStatement"]++;
            }
            //cout<< "Return value: " << variant_to_string(returnValue) << endl; // Debug
            return returnValue; // return value from the function
        } else if (auto fc = dynamic_cast<FunctionCall*>(node)) {
            auto start = high_resolution_clock::now();
            //print->expr = simplify(print->expr);
            vector<Value> args;
            for (auto* arg : fc->args) {
                args.push_back(arg->eval());
            }
            if (stdlib.find(fc->name)!=stdlib.end()){
                Value result = stdlib[fc->name](args);
                /*if (result!=Value{RETURN_STANDARD}) {
                    return result;
                }*/
            } else {
                for (const auto& funcDef : functionDefinitions) {
                    if (auto* func = dynamic_cast<FunctionDefinition*>(funcDef)) {
                        if (func->name == fc->name) {
                            Environment* previous = currentEnv;

                            currentEnv = new Environment(previous);
                            for (size_t i = 0; i < func->args.size() && i < args.size(); ++i) {
                                if (auto varDecl = dynamic_cast<VariableDeclaration*>(func->args[i])) {
                                    currentEnv->define(varDecl->name, args[i]);
                                    currentEnv->defineType(varDecl->name, varDecl->type);
                                }
                            }

                            Value res = interpret(func->block, false, profiler, false);

                            Environment* toDelete = currentEnv;
                            currentEnv = previous;

                            delete toDelete;

                            if (res != Value{RETURN_STANDARD}) {
                                return res;
                            }
                        }
                    }
                }
            }
                //cout << "Function call result: " << variant_to_string(result) << endl;  // Debug
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start);
                node_times["FunctionCall"] += duration;
                node_counts["FunctionCall"]++;
            }
        } else if (auto inp = dynamic_cast<InputStatement*>(node)) {
            auto start = high_resolution_clock::now();
            string inputValue;
            getline(cin, inputValue);
            Value inputValueVariant = inputValue;

            currentEnv->define(variant_to_string(inp->name), inputValueVariant);
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start);
                node_times["InputStatement"] += duration;
                node_counts["InputStatement"]++;
            }
        } else if (auto assign = dynamic_cast<AssignStatement*>(node)) {
            auto start = high_resolution_clock::now();
            //assign->expr = simplify(assign->expr);
            Value val = assign->expr->eval();
            if (currentEnv->getType(assign->name) == "NDT") {
                if (assign->index) {
                    vector<Expr*> index = assign->indexList;
                    
                    Value listValue = currentEnv->get(assign->name);
                    RecursiveValue current = listValue;

                    for (size_t i = 0; i < index.size() - 1; ++i) {
                        int idx = std::get<int>(index[i]->eval());
                        auto& vecPtr = std::get<std::shared_ptr<std::vector<RecursiveValue>>>(current);
                        current = (*vecPtr)[idx];
                    }

                    int lastIdx = std::get<int>(index.back()->eval());
                    auto& vecPtr = std::get<std::shared_ptr<std::vector<RecursiveValue>>>(current);
                    (*vecPtr)[lastIdx] = val;

                } else currentEnv->set(assign->name,val);
            } else if (currentEnv->getType(assign->name) == "INT") {
                if (holds_alternative<float>(val)) {
                    currentEnv->set(assign->name,static_cast<int>(get<float>(val)));
                } else if (holds_alternative<string>(val)) {
                    currentEnv->set(assign->name,stoi(get<string>(val)));
                } else if (holds_alternative<int>(val)) {
                    currentEnv->set(assign->name,get<int>(val));
                } else {
                    cout<<"No known conversion for variable: " << assign->name << endl;
                }
            } else if (currentEnv->getType(assign->name) == "FLOAT") {
                if (holds_alternative<int>(val)) {
                    currentEnv->set(assign->name,static_cast<float>(get<int>(val)));
                } else if (holds_alternative<string>(val)) {
                    currentEnv->set(assign->name,stof(get<string>(val)));
                } else if (holds_alternative<float>(val)) {
                    currentEnv->set(assign->name,get<float>(val));
                } else {
                    cout << "No known conversion for variable: " << assign->name << endl;
                }
            } else if (currentEnv->getType(assign->name) == "STRING") {
                if (holds_alternative<int>(val)) {
                    currentEnv->set(assign->name,to_string(get<int>(val)));
                } else if (holds_alternative<bool>(val)) {
                    currentEnv->set(assign->name,to_string(get<bool>(val)));
                } else if (holds_alternative<float>(val)) {
                    currentEnv->set(assign->name,to_string(get<float>(val)));
                } else if (holds_alternative<string>(val)) {
                    currentEnv->set(assign->name,get<string>(val));
                } else {
                    cout << "No known conversion for variable: " << assign->name << endl;
                }
            } else if (currentEnv->getType(assign->name) == "BOOL") {
                currentEnv->set(assign->name,condition_to_bool(val));
            } else {
                cout << "Unknown type for variable: " << assign->name << endl;
                continue; // skip this assignment if type is unknown
            }
            
            //cout << "Assign: " << assign->name << " = " << variant_to_string(variables[assign->name]) << endl;  // Debug
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start);
                node_times["AssignStatement"] += duration;
                node_counts["AssignStatement"]++;
            }
        } else if (auto whileStmt = dynamic_cast<WhileStatement*>(node)) {
            auto start = high_resolution_clock::now();
            auto duration_block = duration_cast<microseconds>(start - start);

            Environment* previous = currentEnv;
            currentEnv = new Environment(previous); // NEW SCOPE

            while (true) {
                Value conditionValue = whileStmt->expr->eval();

                if (!condition_to_bool(conditionValue)) break;

                auto block_start = high_resolution_clock::now();

                Value innerRet = interpret(whileStmt->block, false, profiler, false);

                if (innerRet != Value{RETURN_STANDARD}) return innerRet;
                auto block_end = high_resolution_clock::now();
                duration_block += duration_cast<microseconds>(block_end - block_start);
            }

            delete currentEnv; // delete the scope after execution
            currentEnv = previous; // restore previous scope

            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start - duration_block);
                node_times["WhileStatement"] += duration;
                node_counts["WhileStatement"]++;
            }
        } else if (auto doWhileStmt = dynamic_cast<DoWhileStatement*>(node)) {
            auto start = high_resolution_clock::now();
            auto duration_block = duration_cast<microseconds>(start - start);
            
            do {
                auto block_start = high_resolution_clock::now();

                Environment* previous = currentEnv;
                currentEnv = new Environment(previous); // NEW SCOPE

                Value innerRet = interpret(doWhileStmt->block, false, profiler, false);

                delete currentEnv; // delete the scope after execution
                currentEnv = previous; // restore previous scope

                if (innerRet!=Value{RETURN_STANDARD}) return innerRet;
                auto block_end = high_resolution_clock::now();
                duration_block += duration_cast<microseconds>(block_end - block_start);

                Value conditionValue = doWhileStmt->expr->eval();

                if (!condition_to_bool(conditionValue)) break;
            } while (true);
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start - duration_block);
                node_times["DoWhileStatement"] += duration;
                node_counts["DoWhileStatement"]++;
            }
        } else if (auto doUntilStmt = dynamic_cast<DoUntilStatement*>(node)) {
            auto start = high_resolution_clock::now();
            auto duration_block = duration_cast<microseconds>(start - start);
            
            do {
                auto block_start = high_resolution_clock::now();

                Environment* previous = currentEnv;
                currentEnv = new Environment(previous); // NEW SCOPE

                Value innerRet = interpret(doUntilStmt->block, false, profiler, false);

                delete currentEnv; // delete the scope after execution
                currentEnv = previous; // restore previous scope

                if (innerRet != Value{RETURN_STANDARD}) return innerRet;
                auto block_end = high_resolution_clock::now();
                duration_block += duration_cast<microseconds>(block_end - block_start);

                Value conditionValue = doUntilStmt->expr->eval();

                if (condition_to_bool(conditionValue)) break;
            } while (true);
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start - duration_block);
                node_times["DoUntilStatement"] += duration;
                node_counts["DoUntilStatement"]++;
            }
        } else if (auto forStmt = dynamic_cast<ForStatement*>(node)) {
            auto start = high_resolution_clock::now();
            auto duration_block = duration_cast<microseconds>(start - start);

            interpret({forStmt->init_block}, false, profiler, false);

            Environment* previous = currentEnv;
            currentEnv = new Environment(previous); // NEW SCOPE
            
            Value conditionValue = forStmt->expr->eval();
            while (condition_to_bool(conditionValue)) {
                auto block_start = high_resolution_clock::now();

                
                Value innerRet = interpret(forStmt->block, false, profiler, false);

                if (innerRet != Value{RETURN_STANDARD}) return innerRet;
                interpret({forStmt->assign_block}, false, profiler, false);
                auto block_end = high_resolution_clock::now();
                duration_block += duration_cast<microseconds>(block_end - block_start);
                conditionValue = forStmt->expr->eval(); // re-evaluate condition after each iteration
            }

            delete currentEnv; // delete the scope after execution
            currentEnv = previous; // restore previous scope

            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start - duration_block);
                node_times["ForStatement"] += duration;
                node_counts["ForStatement"]++;
            }
        } else if (auto ifs = dynamic_cast<IfStatement*>(node)) {
            auto start = high_resolution_clock::now();
            //ifs->expr = simplify(ifs->expr);
            Value conditionValue = ifs->expr->eval();
            if (condition_to_bool(conditionValue)) {

                Environment* previous = currentEnv;
                currentEnv = new Environment(previous); // NEW SCOPE

                Value innerRet = interpret(ifs->block,false, profiler, false);

                delete currentEnv; // delete the scope after execution
                currentEnv = previous; // restore previous scope

                if (innerRet != Value{RETURN_STANDARD}) return innerRet;
            } else {
                bool run_first_branch=false;
                for (auto& elseIfBranch : ifs->elseIfBranches) {
                    //elseIfBranch.first = simplify(elseIfBranch.first);
                    Value elseIfConditionValue = elseIfBranch.first->eval();
                    if (condition_to_bool(elseIfConditionValue) && !run_first_branch) {

                        Environment* previous = currentEnv;
                        currentEnv = new Environment(previous); // NEW SCOPE

                        Value innerRet = interpret(elseIfBranch.second,false, profiler, false);

                        delete currentEnv; // delete the scope after execution
                        currentEnv = previous; // restore previous scope

                        if (innerRet != Value{RETURN_STANDARD}) return innerRet;
                        run_first_branch = true;
                        break;
                    }
                }
                if (!ifs->elseBlock.empty() && !run_first_branch) {

                    Environment* previous = currentEnv;
                    currentEnv = new Environment(previous); // NEW SCOPE

                    Value innerRet = interpret(ifs->elseBlock,false, profiler, false);

                    delete currentEnv; // delete the scope after execution
                    currentEnv = previous; // restore previous scope

                    if (innerRet != Value{RETURN_STANDARD}) return innerRet;
                }
            }
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start);
                node_times["IfStatement"] += duration;
                node_counts["IfStatement"]++;
            }
        }
    }

	//cout<<endl;

    //for (const auto& var : variables) {
    //    cout << "Variable: " << var.first << " = " << variant_to_string(var.second) << endl;
    //}

    auto full_interpretation_end = high_resolution_clock::now();
    if (profiler && print_pdata) {
        cout<<endl;
        auto duration = duration_cast<microseconds>(full_interpretation_end - full_interpretation_start);
        //auto durations = duration_cast<seconds>(full_interpretation_end - full_interpretation_start);
        double seconds = duration.count() / 1000000.0;
        cout << "Full interpretation took: " << duration.count() << " micros " << seconds << " s" << endl;
        cout << "Node execution times:" << endl;
        for (const auto& [node_type, time] : node_times) {
            cout << node_type << ": " << time.count() << " micros, executed " << node_counts[node_type] << " times" << endl;
            cout << "Average time: " << (time.count() / (node_counts[node_type] ? node_counts[node_type] : 1)) << " micros" << endl;
        }
    }
    return Value{RETURN_STANDARD};
}
