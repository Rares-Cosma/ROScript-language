#pragma once
#include <variant>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using Value = variant<int, float, string>;

inline string variant_to_string(const Value& v) {
    if (holds_alternative<int>(v)) return to_string(get<int>(v));
    if (holds_alternative<float>(v)) return to_string(get<float>(v));
    if (holds_alternative<string>(v)) return get<string>(v);
    return "NDT"; // handle cases where the type is unsupported
}

class ASTNode {
	public:
		virtual ~ASTNode() = default;
		virtual void get() {}; // default implementation
		virtual Value eval() {}; // pure virtual function for evaluation
};

class Expr: public ASTNode {
	public:
		Value eval() override { cout<<"Base Expression"; }; // pure virtual function for evaluation{
		void get() override {
			cout << "Base Expression"<<endl; // expression
		}
};

// STATEMENTS

class VariableDeclaration : public ASTNode {
	public:
		string name, type;
		Expr* value;
		
	
		VariableDeclaration(string t, string n, Expr* v) : name(n), type(t), value(v) {}
	
		void get() override {
			cout << "Variable Declaration: " << type << " " << name << " = ";
			if (value) {
				Value v = value->eval();
				if (std::holds_alternative<int>(v)) std::cout << std::get<int>(v);
				else if (std::holds_alternative<float>(v)) std::cout << std::get<float>(v);
				else if (std::holds_alternative<string>(v)) std::cout << std::get<string>(v);
				else cout << "unknown";
			} else {
				cout << "NULL";
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
};

class FloatLiteral : public Expr {
    float value;
	public:
    FloatLiteral(float v) : value(v) {}
    Value eval() override { return value; }
};

class StringLiteral : public Expr {
    string value;
	public:
    StringLiteral(string v) : value(move(v)) {}
    Value eval() override { return value; }
};

class VariableRefrence : public Expr {
    string name;
	public:
	VariableRefrence(string v) : name(move(v)) {}
    Value eval() override { return name; }
};

class BinaryExpr : public Expr {
    Expr* left;
    Expr* right;
    std::string op;
	public:
	BinaryExpr(Expr* l, string o, Expr* r) : left(l), op(move(o)), right(r) {}


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
		}

		if (std::holds_alternative<string>(lval) && std::holds_alternative<string>(rval) && op == "+")
		{
			return std::get<string>(lval) + std::get<string>(rval);
		}

		throw std::runtime_error("Unsupported operation or mismatched types");
	}

	~BinaryExpr() {
        delete left;
        delete right;
    }
};

vector<ASTNode*> parse(vector<pair<string, string>> tokens);
