#include "lexer.h"
#include "commons.cpp"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

bool isnotsep(char letter) {
    string sep = " =;+-*/(){}[],<>!\n\r";
    return sep.find(letter) == string::npos;
}

bool iskeyword(const string& word) {
    vector<string> keywords = {"var", "afiseaza","citeste","daca","atunci","altfel","executa","cat","timp","pentru","pana","cand","fiecare"};
    for (const string& kw : keywords) {
        if (word == kw) return true;
    }
    return false;
}

vector<pair<string, string>> lexer(string fn) {
    vector<pair<string, string>> tokens;
    ifstream file(fn);
    Type checker;

    if (!file) {
        cout << "File not found" << endl;
        return tokens;
    }

    char current_char;
    string keyword;
    while (file.get(current_char)) {
        if (current_char == '\n' || current_char == '\r') {
            continue;
        }

        if (current_char == '"') {
    string str_literal = "";
    while (file.get(current_char) && current_char != '"') {
        if (current_char == '\\') {
            // handle escape sequence
            if (file.get(current_char)) {
                switch (current_char) {
                    case 'n': str_literal += '\n'; break;
                    case 't': str_literal += '\t'; break;
                    case '\\': str_literal += '\\'; break;
                    case '"': str_literal += '\"'; break;
                    default: str_literal += current_char; break;
                }
            }
        } else {
            str_literal += current_char;
        }
    }
    tokens.push_back({"STRING", str_literal});
    continue;
}

        if (isnotsep(current_char)) {
            keyword += current_char;
        } else {
            if (!keyword.empty()) {
                if (iskeyword(keyword)) tokens.push_back({"KEYWORD", keyword});
		        else if (checker.is_float_numeral(keyword)) tokens.push_back({"FLOAT", keyword});
                else if (checker.is_integer_numeral(keyword)) tokens.push_back({"INT", keyword});
		        else tokens.push_back({"ID", keyword});
                keyword = "";
            }

            if (current_char == '=' || current_char == '!' || current_char == '<' || current_char == '>') {
                if (file.peek() == '=') {
                    char next_char;
                    file.get(next_char);
                    string op = string(1, current_char) + "=";
                    tokens.push_back({"OP", op});
                } else {
                    tokens.push_back({"OP", string(1, current_char)});
                }
            }
            
            else if (current_char == ';') tokens.push_back({"NLINE", ";"});
            else if (current_char == '+') tokens.push_back({"OP", "+"});
            else if (current_char == '-') tokens.push_back({"OP", "-"});
            else if (current_char == '/') tokens.push_back({"OP", "/"});
            else if (current_char == '*') tokens.push_back({"OP", "*"});
            else if (current_char == '(') tokens.push_back({"LPAREN", "("});
            else if (current_char == ')') tokens.push_back({"RPAREN", ")"});
            else if (current_char == '{') tokens.push_back({"LBRACE", "{"});
            else if (current_char == '}') tokens.push_back({"RBRACE", "}"});
        }
    }

    return tokens;
}
