#include "parser.h"
#include "commons.cpp"

// wrapper class for the lexer output for easier handling of tokens

class TokenStream {
public:
	vector<pair<string, string>> tokens;
	vector<vector<pair<string, string>>> lines;

	void init() {
		vector<pair<string, string>> line;
		for (const auto& token : tokens) {
			if (token.first == "NLINE") {
				lines.push_back(line);
				line.clear();
			} else {
				line.push_back(token);
			}
		}
		if (!line.empty()) {
			lines.push_back(line);
		}
	}
};

// ABSTRACT SYNTAX TREE IMPLEMEMTATION

vector<ASTNode*> AST; // vector of AST nodes

// PARSER IMPLEMENTATION

int get_precedence(const string& op) {
	if (op == "+" || op == "-") return 1;
	if (op == "*" || op == "/") return 2;
	return 0;
}

Expr* parse_expression(const vector<pair<string, string>>& tokens, int& idx);

Expr* parse_primary_expression(const vector<pair<string, string>>& tokens, int& idx) {
    if (idx >= tokens.size()) return nullptr;

    if (tokens[idx].first == "INT") {
        int value = stoi(tokens[idx].second);
        idx++;
        return new IntLiteral(value);
    }
    else if (tokens[idx].first == "FLOAT") {
        float value = stof(tokens[idx].second);
        idx++;
        return new FloatLiteral(value);
    }
    else if (tokens[idx].first == "STRING") {
        string value = tokens[idx].second;
        idx++;
        return new StringLiteral(value);
    }
    else if (tokens[idx].first == "ID") {
        string name = tokens[idx].second;
        idx++;
        return new VariableRefrence(name);
    }
	else if (tokens[idx].first == "LPAREN") {
        idx++; // consume (
        Expr* expr = parse_expression(tokens, idx);
        if (idx >= tokens.size() || tokens[idx].first != "RPAREN") {
            cerr << "Error: expected ')' after expression" << endl;
            return nullptr;
        }
        idx++; // consume )
        return expr;
    }
    return nullptr;
}

Expr* parse_rhs_expression(int expr_prec, Expr* lhs, const vector<pair<string, string>>& tokens, int& idx) {
	while (idx < tokens.size()) {
        if (tokens[idx].first != "OP") break;
        string op = tokens[idx].second;
        int prec = get_precedence(op);

        if (prec < expr_prec) break;

        idx++; // consume operator
        Expr* rhs = parse_primary_expression(tokens, idx);
        if (!rhs) return nullptr;

        while (idx < tokens.size() && tokens[idx].first == "OP" &&
               get_precedence(tokens[idx].second) >= prec) {
            rhs = parse_rhs_expression(get_precedence(tokens[idx].second), rhs, tokens, idx);
        }

        lhs = new BinaryExpr(lhs, op, rhs);
    }

    return lhs;
}

Expr* parse_expression(const vector<pair<string, string>>& tokens, int& idx) {
	Expr* left = parse_primary_expression(tokens, idx);
	if (!left) return nullptr;

	return parse_rhs_expression(0, left, tokens, idx);
}

void report_error(const string& msg, const vector<pair<string, string>>& line, int line_nb) {
	/**
 	* @brief Thows custom syntax errors.
 	* @param line The line of the error.
 	* @param line_nb The line number in the source code.
 	* @return Prints the error message and the line of code.
	 */
	cerr << "Syntax Error: " << msg << "\nOn line: ";
	cout << line_nb << ": ";
	for (const auto& token : line) {
		cerr << token.second << " ";
	}
	cerr << "\n\n";
}

void parse_variable_declaration(const vector<pair<string, string>>& line, int line_nb) {
	/**
 	* @brief Parses a variable declaration line.
 	* @param line The line to parse.
 	* @param line_nb The line number in the source code.
 	* @return Adds the variable declaration to the AST.
 	* @note Because "var" is not a type spefcific keyword, we cannot determine the type of the variable, in case it has no initializer so we add it to the NDT stack (non determined) and relocate it later.
	 */
	if (line.size() < 2) {
		report_error("Expected identifier after 'var'", line, line_nb);
		return;
	}

	if (line[1].first != "ID") {
		report_error("Expected variable name after 'var'", line, line_nb);
		return;
	}

	if (line.size() == 2) {
		ASTNode* node = new VariableDeclaration("NDT", line[1].second, nullptr);
		AST.push_back(node);
		return;
	}

	if (line[2].second == "=") {
		if (line.size() > 3) {
			int idx = 3;
			Expr* expr = parse_expression(line, idx);
			
			if (expr) {
				ASTNode* node = new VariableDeclaration("NDT", line[1].second, expr);
				AST.push_back(node);
			} else {
				report_error("Expression parsing failed", line, line_nb);
			}
		} else {
			report_error("Expected initializer after '='", line, line_nb);
		}
	} else {
		report_error("Expected '=' after variable name", line, line_nb);
	}
}

vector<ASTNode*> parse(vector<pair<string, string>> tokens) {
	TokenStream stream;
	stream.tokens = tokens;
	stream.init();

	int ct = 1;

	for (const auto& line : stream.lines) {
		if (line.empty()) continue;

		const auto& first_token = line[0];
		if (first_token.first == "KEYWORD" && first_token.second == "var") {
			parse_variable_declaration(line, ct);
		} else {
			// handle other types of lines
		}
		ct++;
	}
	return AST;
}

