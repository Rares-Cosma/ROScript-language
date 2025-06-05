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


void interpret(std::vector<ASTNode*> AST) {
    for (ASTNode* node : AST) {
        if (auto varDecl = dynamic_cast<VariableDeclaration*>(node)) {
            varDecl->value = simplify(varDecl->value);
            Value value = varDecl->value->eval();
            variables[varDecl->name] = value;
        } else if (auto print = dynamic_cast<PrintStatement*>(node)) {
            print->expr = simplify(print->expr);
            cout<<variant_to_string(print->expr->eval());
        } else if (auto inp = dynamic_cast<InputStatement*>(node)) {
            string inputValue;
            getline(cin, inputValue);
            Value inputValueVariant = inputValue;

            variables[variant_to_string(inp->expr->eval())] = inputValueVariant;
        } else if (auto assign = dynamic_cast<AssignStatement*>(node)) {
            assign->expr = simplify(assign->expr);
            variables[assign->name] = assign->expr->eval();
        } else if (auto ifs = dynamic_cast<IfStatement*>(node)) {
            ifs->expr = simplify(ifs->expr);
            Value conditionValue = ifs->expr->eval();
            if (holds_alternative<bool>(conditionValue) && get<bool>(conditionValue)) {
                interpret(ifs->block);
            }
        }
    }

	cout<<endl;

    //for (const auto& var : variables) {
    //    cout << "Variable: " << var.first << " = " << variant_to_string(var.second) << endl;
    //}
}
