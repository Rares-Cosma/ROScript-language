#pragma once
#include "stdlib.cpp"
#include "functionCall.h"
#include <vector>
#include "errors.h"
#include "ansi.h"
#include "variables.h"
#include <iostream>
#include <algorithm>

inline vector<string> arithmetic_operators = {"+", "-", "*", "/", "%"};
inline vector<string> comparison_operators = {"==", "!=", "<", ">", "<=", ">=", "&&", "||"};

inline string variant_to_string(const Value& v) {
    if (holds_alternative<int>(v)) return to_string(get<int>(v));
    if (holds_alternative<float>(v)) return to_string(get<float>(v));
    if (holds_alternative<string>(v)) return get<string>(v);
	if (holds_alternative<bool>(v)) return to_string(get<bool>(v));
    return "NDT"; // handle cases where the type is unsupported
}

class ASTNode {
	public:
		virtual ~ASTNode() = default;
		virtual void get(int indent = 0) const = 0;
		virtual Value eval() {}; // pure virtual function for evaluation
		int line_nb = 0; // line number in the source code
		string line; // line of the source code
};

class Expr: public ASTNode {
	public:
		Value eval() override { cout<<"Base Expression"; }; // pure virtual function for evaluation
		virtual void print() const = 0;
		virtual Expr* clone() const = 0;
		void get(int indent = 0) const override {}
};

// STATEMENTS

class PrintStatement : public ASTNode {
	public:
		Expr* expr;
	
		PrintStatement(Expr* e) : expr(e) {}
	
		void get(int indent=0) const override {
			cout << "Print Statement: ";
			expr->get();
			cout << endl;
		}
	
		~PrintStatement() {
			delete expr;
		}
	};

class AssignStatement : public ASTNode {
	public:
		Expr* expr;
		string name;
		bool index;
		vector<Expr*> indexList; // for list indexing
		
		AssignStatement(Expr* e, string name, bool i, vector<Expr*> il) : expr(e), name(name), index(i), indexList(il) {}
		
		void get(int indent=0) const override {
			cout << "Assignment Statement: ";
			cout << name << " = ";
			expr->get();
			cout << endl;
		}
		
		~AssignStatement() {
			delete expr;
		}
	};

class IfStatement : public ASTNode {
public:
    Expr* expr;
    vector<ASTNode*> block;
    vector<pair<Expr*, vector<ASTNode*>>> elseIfBranches;
    vector<ASTNode*> elseBlock;

    IfStatement(Expr* e, vector<ASTNode*> block,
                vector<pair<Expr*, vector<ASTNode*>>> elseIfBranches = {},
                vector<ASTNode*> elseBlock = {})
        : expr(e), block(block),
          elseIfBranches(elseIfBranches), elseBlock(elseBlock) {}

    void get(int indent = 0) const override {
        cout << string(indent, ' ') << "If Statement:\n";

        cout << string(indent + 2, ' ') << "Condition: ";
        expr->get();

        cout << string(indent + 2, ' ') << "Block:\n";
        for (const auto& node : block) {
            node->get(indent + 4);
        }

        for (const auto& branch : elseIfBranches) {
            cout << string(indent + 2, ' ') << "Else If Condition: ";
            branch.first->get();
            cout << string(indent + 2, ' ') << "Block:\n";
            for (const auto& node : branch.second) {
                node->get(indent + 4);
            }
        }

        if (!elseBlock.empty()) {
            cout << string(indent + 2, ' ') << "Else Block:\n";
            for (const auto& node : elseBlock) {
                node->get(indent + 4);
            }
        }
        cout << endl;
    }

    ~IfStatement() {
        delete expr;
        for (auto& branch : elseIfBranches) {
            delete branch.first;
            for (auto node : branch.second)
                delete node;
        }
        for (auto node : block)
            delete node;
        for (auto node : elseBlock)
            delete node;
    }
};

class WhileStatement : public ASTNode {
	public:
		Expr* expr;
		vector<ASTNode*> block;
		
		WhileStatement(Expr* e, vector<ASTNode*> block) : expr(e), block(block) {}
		
		void get(int indent=0) const override {
			cout << "While Statement: ";
			cout << string(indent + 2, ' ') << "Block:\n";
        	for (const auto& node : block) {
            	node->get(indent + 4);
        	}
			cout << endl;
		}
		
		~WhileStatement() {
			delete expr;
		}
	};

class DoWhileStatement : public ASTNode {
	public:
		Expr* expr;
		vector<ASTNode*> block;
		
		DoWhileStatement(Expr* e, vector<ASTNode*> block) : expr(e), block(block) {}
		
		void get(int indent=0) const override {
			cout << "Do while Statement: ";
			cout << string(indent + 2, ' ') << "Block:\n";
        	for (const auto& node : block) {
            	node->get(indent + 4);
        	}
			cout << endl;
		}
		
		~DoWhileStatement() {
			delete expr;
		}
	};

class DoUntilStatement : public ASTNode {
	public:
		Expr* expr;
		vector<ASTNode*> block;
		
		DoUntilStatement(Expr* e, vector<ASTNode*> block) : expr(e), block(block) {}
		
		void get(int indent=0) const override {
			cout << "Do until Statement: ";
			cout << string(indent + 2, ' ') << "Block:\n";
        	for (const auto& node : block) {
            	node->get(indent + 4);
        	}
			cout << endl;
		}
		
		~DoUntilStatement() {
			delete expr;
		}
	};

class ForStatement : public ASTNode {
	public:
		Expr* expr;
		vector<ASTNode*> block;
		ASTNode *init_block,*assign_block;
		
		ForStatement(ASTNode* ib, Expr* e, vector<ASTNode*> block, ASTNode* ab) : init_block(ib), expr(e), block(block), assign_block(ab) {}
		
		void get(int indent=0) const override {
			cout << "For Statement: ";
			cout << string(indent + 2, ' ') << "Block:\n";
        	for (const auto& node : block) {
            	node->get(indent + 4);
        	}
			cout << endl;
		}
		
		~ForStatement() {
			delete expr;
		}
	};

class InputStatement : public ASTNode {
	public:
		string name;
		
		InputStatement(string e) : name(e) {}
		
		void get(int indent=0) const override {
			cout << "Input Statement: ";
			cout<<name;
			cout << endl;
		}
	};

class ReturnStatement : public ASTNode {
	public:
		Expr* expr;
		
		ReturnStatement(Expr* e) : expr(e) {}
		
		void get(int indent=0) const override {
			cout << "Return Statement: ";
			if (expr) {
				expr->get();
			} else {
				cout << "void";
			}
			cout << endl;
		}
		
		~ReturnStatement() {
			delete expr;
		}
	};

class FunctionDefinition : public ASTNode {
	public:
	string name;
	vector<ASTNode*> args;
	vector<ASTNode*> block;
	FunctionDefinition(string n,vector<ASTNode*> a, vector<ASTNode*> b) : name(move(n)), args(move(a)), block(move(b)) {}
	void get(int indent=0) const override {}
};

extern vector<ASTNode*> functionDefinitions;

inline Value callFunction(const string& name, const vector<Value>& args){
	auto it = stdlib.find(name);
    if (it != stdlib.end()) {
		return it->second(args);
    }
	for (const auto& funcDef : functionDefinitions) {
		if (auto* func = dynamic_cast<FunctionDefinition*>(funcDef)) {
			if (func->name == name) {
				return functionCallEval(func->args, args, func->block);
			}
		}
	}
	throw Error(
		colorize("Eroare de apelare a functiei: ", Color::Red, 0) + 
		"Funcția '" + name + "' nu a fost găsită în biblioteca standard sau în definițiile de funcții.",
		CURRENT_FILE,
		0,
		"");
}

class FunctionCall : public Expr {
	public:
	string name;
	vector<Expr*> args;
	FunctionCall(string v, vector<Expr*> a) : name(move(v)), args(move(a)) {}
    Value eval() override {
		vector<Value> argValues;
		for (auto* arg : args) {
			argValues.push_back(arg->eval());
		}

		return callFunction(name, argValues);
	}
	void get(int indent = 0) const override {}
	Expr* clone() const override {
    	vector<Expr*> clonedArgs;
    	for (auto* arg : args) {
        	clonedArgs.push_back(arg->clone());
    	}
    	return new FunctionCall(name, move(clonedArgs));
	}
	void print() const override {
	}
};

class VariableDeclaration : public ASTNode {
	public:
		string name, type;
		Expr* value;
		
	
		VariableDeclaration(string t, string n, Expr* v) : name(n), type(t), value(v) {}
	
		void get(int indent=0) const override {
			cout << "Variable Declaration: " << type << " " << name << " = ";
			if (value) {
				value->get();
			} else {
				cout << "NDT"; // No default type
			}
			cout << endl;
		}
	
		~VariableDeclaration() {
			delete value;
		}
	};

// LITERALS

class IntLiteral : public Expr {
    int value;
	public:
    IntLiteral(int v) : value(v) {}
    Value eval() override { return value; }
	void get(int indent = 0) const override {}
	Expr* clone() const override {
        return new IntLiteral(value);
    }
	void print() const override {
		cout << value;
	}
};

class BoolLiteral : public Expr {
    bool value;
	public:
    BoolLiteral(bool v) : value(v) {}
    Value eval() override { return value; }
	void get(int indent = 0) const override {}
	Expr* clone() const override {
        return new BoolLiteral(value);
    }
	void print() const override {
		cout << value;
	}
};

class FloatLiteral : public Expr {
    float value;
	public:
    FloatLiteral(float v) : value(v) {}
    Value eval() override { return value; }
	void get(int indent = 0) const override {}
	Expr* clone() const override {
        return new FloatLiteral(value);
    }
	void print() const override {
		cout << value;
	}
};

class StringLiteral : public Expr {
    string value;
	public:
    StringLiteral(string v) : value(move(v)) {}
    Value eval() override { return value; }
	void get(int indent = 0) const override {}
	Expr* clone() const override {
        return new StringLiteral(value);
    }
	void print() const override {
		cout << value;
	}
};

class ListLiteral : public Expr {
	public:
	vector<Expr*> args;
	unsigned long long size;
	ListLiteral(vector<Expr*> v, unsigned long long s) : args(move(v)), size(move(s)) {}
	Value eval() override {
		vector<RecursiveValue> values;
    	for (auto* arg : args) {
        	values.push_back(static_cast<RecursiveValue>(arg->eval()));
    	}
    	return Value{ std::make_shared<std::vector<RecursiveValue>>(std::move(values)) };
	}
	void get(int indent = 0) const override {}
	Expr* clone() const override {
		return new ListLiteral(args, size);
	}
	void print() const override {
	}
};

class ListIndex : public Expr {
	public:
	string name;
	vector<Expr*> index;
	ListIndex(string l, vector<Expr*> i) : name(l), index(i) {}
	Value eval() override {
		Value listValue = currentEnv->get(name);
        if (std::holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(listValue)) {
        	RecursiveValue current = listValue;

        	for (Expr* expr : index) {
            	int idx = std::get<int>(expr->eval());

            	auto& vecPtr = std::get<std::shared_ptr<std::vector<RecursiveValue>>>(current);
            	current = (*vecPtr)[idx];
        	}

        	return current;
        } else {
            throw Error(
                colorize("Eroare de tip 002: ", Color::Red, 0) +
                "Valoarea nu este o lista:" + name,
                CURRENT_FILE,
                0,
                "");
        }
    }
	void get(int indent = 0) const override {}
	Expr* clone() const override {}
	void print() const override {}
};

class Refrence : public Expr {
    string name;
	public:
	Refrence(string v) : name(move(v)) {}
    Value eval() override { return currentEnv->get(name); }
	void get(int indent = 0) const override {}
	Expr* clone() const override {
        return new Refrence(name);
    }
	void print() const override {
		cout << name;
	}
};

class UnaryExpr : public Expr {
public:
    string op;
    Expr* expr;

    UnaryExpr(const string& op, Expr* expr) : op(op), expr(expr) {}

    Value eval() override {
        Value val = expr->eval();

        if (op == "-") {
            if (holds_alternative<int>(val)) return -std::get<int>(val);
            if (holds_alternative<float>(val)) return -std::get<float>(val);
        } else if (op == "!") {
            if (holds_alternative<bool>(val)) return !std::get<bool>(val);
            if (holds_alternative<int>(val)) return !std::get<int>(val);
        }

        throw Error(
			colorize("Eroare de evaluare a expresiei: ", Color::Red, 0) + 
			"Operatie unara invalida pentru tipul dat: " + 
			op + " " + variant_to_string(val),
			CURRENT_FILE,
			0,
			"");
    }
	Expr* clone() const override {}
	void print() const override {
		cout << op;
	}
};


class BinaryExpr : public Expr {
	public:
	Expr* left;
    Expr* right;
    std::string op;
	BinaryExpr(Expr* l, string o, Expr* r) : left(l), op(move(o)), right(r) {}

	void get(int indent = 0) const override {}

	void print() const override {
		cout << "BinaryExpr(";
		left->print();
		cout << " " << op << " ";
		right->print();
		cout << ")";
	}

	Expr* clone() const override {
        return new BinaryExpr(left->clone(), op, right->clone());
    }

	Value eval() override
	{
		Value lval = left->eval();
		Value rval = right->eval();

		if (std::holds_alternative<int>(lval) && std::holds_alternative<int>(rval))
		{
			if (op == "+")
				return std::get<int>(lval) + std::get<int>(rval);
			if (op == "-")
				return std::get<int>(lval) - std::get<int>(rval);
			if (op == "*")
				return std::get<int>(lval) * std::get<int>(rval);
			if (op == "/")
				return std::get<int>(lval) / std::get<int>(rval);
			if (op == "%")
				return std::get<int>(lval) % std::get<int>(rval);
			if (op == "==")
				return std::get<int>(lval) == std::get<int>(rval);
			if (op == "!=")
				return std::get<int>(lval) != std::get<int>(rval);
			if (op == "<")
				return std::get<int>(lval) < std::get<int>(rval);
			if (op == ">")
				return std::get<int>(lval) > std::get<int>(rval);
			if (op == "<=")
				return std::get<int>(lval) <= std::get<int>(rval);
			if (op == ">=")
				return std::get<int>(lval) >= std::get<int>(rval);
			if (op == "&&")
				return std::get<int>(lval) && std::get<int>(rval);
			if (op == "||")
				return std::get<int>(lval) || std::get<int>(rval);
		}

		if (std::holds_alternative<float>(lval) && std::holds_alternative<float>(rval))
		{
			if (op == "+")
				return std::get<float>(lval) + std::get<float>(rval);
			if (op == "-")
				return std::get<float>(lval) - std::get<float>(rval);
			if (op == "*")
				return std::get<float>(lval) * std::get<float>(rval);
			if (op == "/")
				return std::get<float>(lval) / std::get<float>(rval);
			if (op == "==")
				return std::get<float>(lval) == std::get<float>(rval);
			if (op == "!=")
				return std::get<float>(lval) != std::get<float>(rval);
			if (op == "<")
				return std::get<float>(lval) < std::get<float>(rval);
			if (op == ">")
				return std::get<float>(lval) > std::get<float>(rval);
			if (op == "<=")
				return std::get<float>(lval) <= std::get<float>(rval);
			if (op == ">=")
				return std::get<float>(lval) >= std::get<float>(rval);
		}

		if ((std::holds_alternative<int>(lval) && std::holds_alternative<float>(rval)) ||
			(std::holds_alternative<float>(lval) && std::holds_alternative<int>(rval)))
		{
			float lval_f = std::holds_alternative<int>(lval) ? static_cast<float>(std::get<int>(lval)) : std::get<float>(lval);
			float rval_f = std::holds_alternative<int>(rval) ? static_cast<float>(std::get<int>(rval)) : std::get<float>(rval);

			if (op == "+")
				return lval_f + rval_f;
			if (op == "-")
				return lval_f - rval_f;
			if (op == "*")
				return lval_f * rval_f;
			if (op == "/")
				return lval_f / rval_f;
			if (op == "==")
				return lval_f == rval_f;
			if (op == "!=")
				return lval_f != rval_f;
			if (op == "<")
				return lval_f < rval_f;
			if (op == ">")
				return lval_f > rval_f;
			if (op == "<=")
				return lval_f <= rval_f;
			if (op == ">=")
				return lval_f >= rval_f;
		}

		if (std::holds_alternative<string>(lval) && std::holds_alternative<string>(rval))
		{
			if (op=="+")
				return std::get<string>(lval) + std::get<string>(rval);
			if (op == "==")
				return std::get<string>(lval) == std::get<string>(rval);
			if (op == "!=")
				return std::get<string>(lval) != std::get<string>(rval);
		}

		if (std::holds_alternative<bool>(lval) && std::holds_alternative<bool>(rval))
		{
			if (op == "==")
				return std::get<bool>(lval) == std::get<bool>(rval);
			if (op == "!=")
				return std::get<bool>(lval) != std::get<bool>(rval);
			if (op == "&&")
				return std::get<bool>(lval) && std::get<bool>(rval);
			if (op == "||")
				return std::get<bool>(lval) || std::get<bool>(rval);
		}

		throw Error(
			colorize("Eroare de evaluare a expresiei: ", Color::Red, 0) + 
			"Operatie invalida pentru tipurile date: " + 
			to_string(lval.index()) + " " + op + " " + to_string(rval.index()),
			CURRENT_FILE,
			0,
			"");
	}

	~BinaryExpr() {
        delete left;
        delete right;
    }
};

vector<ASTNode*> parse(vector<pair<string, string>> tokens, vector<int> tokens_per_line);
