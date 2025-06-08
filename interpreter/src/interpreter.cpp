#include "interpreter.h"
#include <unordered_map>

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

void interpret(std::vector<ASTNode*> AST, bool fprint_ast = false) {
    if (fprint_ast){
        cout << "AST:" << endl;
        print_ast(AST);
    }
    //cout<<AST.size()<<" AST nodes found."<<endl;

    for (ASTNode* node : AST) {
        if (auto varDecl = dynamic_cast<VariableDeclaration*>(node)) {
            //varDecl->value = simplify(varDecl->value);
            Value value = varDecl->value->eval();
            variables[varDecl->name] = value;
        } else if (auto print = dynamic_cast<PrintStatement*>(node)) {
            //print->expr = simplify(print->expr);
            cout<<variant_to_string(print->expr->eval());
        } else if (auto inp = dynamic_cast<InputStatement*>(node)) {
            string inputValue;
            getline(cin, inputValue);
            Value inputValueVariant = inputValue;

            variables[variant_to_string(inp->expr->eval())] = inputValueVariant;
        } else if (auto assign = dynamic_cast<AssignStatement*>(node)) {
            //assign->expr = simplify(assign->expr);
            variables[assign->name] = assign->expr->eval();
            //cout << "Assign: " << assign->name << " = " << variant_to_string(variables[assign->name]) << endl;  // Debug
        } else if (auto whileStmt = dynamic_cast<WhileStatement*>(node)) {
            while (true) {
                Value conditionValue = whileStmt->expr->eval();

                if (!condition_to_bool(conditionValue)) break;

                interpret(whileStmt->block, false);
            }
        } else if (auto ifs = dynamic_cast<IfStatement*>(node)) {
            //ifs->expr = simplify(ifs->expr);
            Value conditionValue = ifs->expr->eval();
            if (condition_to_bool(conditionValue)) {
                interpret(ifs->block,false);
            } else {
                bool run_first_branch=false;
                for (auto& elseIfBranch : ifs->elseIfBranches) {
                    //elseIfBranch.first = simplify(elseIfBranch.first);
                    Value elseIfConditionValue = elseIfBranch.first->eval();
                    if (condition_to_bool(elseIfConditionValue) && !run_first_branch) {
                        interpret(elseIfBranch.second,false);
                        run_first_branch = true;
                        break;
                    }
                }
                if (!ifs->elseBlock.empty() && !run_first_branch) {
                    interpret(ifs->elseBlock,false);
                }
            }
        }
    }

	//cout<<endl;

    //for (const auto& var : variables) {
    //    cout << "Variable: " << var.first << " = " << variant_to_string(var.second) << endl;
    //}
}
