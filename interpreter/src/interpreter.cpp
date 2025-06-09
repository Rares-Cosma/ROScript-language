#include "interpreter.h"
#include <unordered_map>
#include <chrono>

using namespace chrono;

unordered_map<string,Value> variables; // vector of variables

Expr* simplify(Expr* expr) {
    if (auto ref = dynamic_cast<Refrence*>(expr)) {
        string refName = get<string>(ref->eval());

        if (variables.find(refName) != variables.end()) {
            Value varValue = variables[refName];

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

void interpret(std::vector<ASTNode*> AST, bool fprint_ast, bool profiler, bool print_pdata) {
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
            variables[varDecl->name] = value;
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
        } else if (auto fc = dynamic_cast<FunctionCall*>(node)) {
            auto start = high_resolution_clock::now();
            //print->expr = simplify(print->expr);
            vector<Value> args;
            for (auto* arg : fc->args) {
                args.push_back(arg->eval());
            }
            Value result = stdlib[fc->name](args);
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

            variables[variant_to_string(inp->name)] = inputValueVariant;
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start);
                node_times["InputStatement"] += duration;
                node_counts["InputStatement"]++;
            }
        } else if (auto assign = dynamic_cast<AssignStatement*>(node)) {
            auto start = high_resolution_clock::now();
            //assign->expr = simplify(assign->expr);
            variables[assign->name] = assign->expr->eval();
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
            while (true) {
                Value conditionValue = whileStmt->expr->eval();

                if (!condition_to_bool(conditionValue)) break;

                auto block_start = high_resolution_clock::now();
                interpret(whileStmt->block, false, profiler, false);
                auto block_end = high_resolution_clock::now();
                duration_block += duration_cast<microseconds>(block_end - block_start);
            }
            auto end = high_resolution_clock::now();
            if (profiler) {
                auto duration = duration_cast<microseconds>(end - start - duration_block);
                node_times["WhileStatement"] += duration;
                node_counts["WhileStatement"]++;
            }
        } else if (auto ifs = dynamic_cast<IfStatement*>(node)) {
            auto start = high_resolution_clock::now();
            //ifs->expr = simplify(ifs->expr);
            Value conditionValue = ifs->expr->eval();
            if (condition_to_bool(conditionValue)) {
                interpret(ifs->block,false, profiler, false);
            } else {
                bool run_first_branch=false;
                for (auto& elseIfBranch : ifs->elseIfBranches) {
                    //elseIfBranch.first = simplify(elseIfBranch.first);
                    Value elseIfConditionValue = elseIfBranch.first->eval();
                    if (condition_to_bool(elseIfConditionValue) && !run_first_branch) {
                        interpret(elseIfBranch.second,false, profiler, false);
                        run_first_branch = true;
                        break;
                    }
                }
                if (!ifs->elseBlock.empty() && !run_first_branch) {
                    interpret(ifs->elseBlock,false, profiler, false);
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
}
