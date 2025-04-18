#include "parser.h"
using namespace std;

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

void report_error(const string& msg, const vector<pair<string, string>>& line, int line_nb) {
	cerr << "Syntax Error: " << msg << "\nOn line: ";
	cout << line_nb << ": ";
	for (const auto& token : line) {
		cerr << token.second << " ";
	}
	cerr << "\n\n";
}

void parse_variable_declaration(const vector<pair<string, string>>& line, int line_nb) {
	if (line.size() < 2) {
		report_error("Expected identifier after 'var'", line, line_nb);
		return;
	}

	if (line[1].first != "ID") {
		report_error("Expected variable name after 'var'", line, line_nb);
		return;
	}

	if (line.size() == 2) {
		// e.g. var x
		cout << "Declare variable '" << line[1].second << "' with no initializer.\n";
		return;
	}

	if (line[2].second == "=") {
		// e.g. var x = ...
		cout << "Declare variable '" << line[1].second << "' with initializer.\n";
		// TODO: parse expression on the right-hand side
	} else {
		report_error("Expected '=' after variable name", line, line_nb);
	}
}

void parse(vector<pair<string, string>> tokens) {
	TokenStream stream;
	stream.tokens = std::move(tokens);
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
}

