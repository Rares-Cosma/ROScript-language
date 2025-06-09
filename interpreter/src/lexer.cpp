#include "lexer.h"
#include "commons.cpp"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

bool isnotsep(char letter) {
    string sep = " =;+-*/(){}[],<>!%\n\r";
    return sep.find(letter) == string::npos;
}

bool iskeyword(const string& word) {
    vector<string> keywords = {"var", "afiseaza","citeste","daca","atunci","altfel","executa","cat","timp","pentru","pana","cand","fiecare"};
    for (const string& kw : keywords) {
        if (word == kw) return true;
    }
    return false;
}

pair<vector<pair<string, string>>,vector<int>> lexer(string fn) {
    vector<pair<string, string>> tokens;
    ifstream file(fn);
    Type checker;

    vector<int> tpl; // to store the number of tokens per line

    int ct=0;

    if (!file) {
        cout << "File not found" << endl;
        return {tokens, tpl};
    }

    char current_char;
    string keyword;
    while (file.get(current_char)) {
        if (current_char == '\r') {
            continue;
        }
        if (current_char == '\n'){
            tpl.push_back(ct);
            ct=0;
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
            ct++;
            continue;
        }

        if (isnotsep(current_char)) {
            keyword += current_char;
        } else {
            if (!keyword.empty()) {
                if (iskeyword(keyword)) {tokens.push_back({"KEYWORD", keyword});ct++;}
		        else if (checker.is_float_numeral(keyword)) {tokens.push_back({"FLOAT", keyword});ct++;}
                else if (checker.is_integer_numeral(keyword)) {tokens.push_back({"INT", keyword});ct++;}
		        else {tokens.push_back({"ID", keyword});ct++;}
                keyword = "";
            }

            if (current_char == '=' || current_char == '!' || current_char == '<' || current_char == '>' || current_char == '+' || current_char == '-' || current_char == '*' || current_char == '/') {
                if (file.peek() == '=') {
                    char next_char;
                    file.get(next_char);
                    string op = string(1, current_char) + "=";
                    tokens.push_back({"OP", op});
                    ct++;
                } else {
                    if (current_char == '+' && file.peek() == '+') {
                        char next_char;
                        file.get(next_char);
                        tokens.push_back({"OP", "++"});
                        ct++;
                    } else if (current_char == '-' && file.peek() == '-') {
                        char next_char;
                        file.get(next_char);
                        tokens.push_back({"OP", "--"});
                        ct++;
                    } else {
                        tokens.push_back({"OP", string(1, current_char)});
                        ct++;
                    }
                }
            }
            
            else if (current_char == ';') {tokens.push_back({"NLINE", ";"});ct++;}
            else if (current_char == '[') {tokens.push_back({"OP", "%"});ct++;}
            else if (current_char == '[') {tokens.push_back({"LBRACKET", "["});ct++;}
            else if (current_char == ']') {tokens.push_back({"RBRACKET", "]"});ct++;}
            else if (current_char == '(') {tokens.push_back({"LPAREN", "("});ct++;}
            else if (current_char == ')') {tokens.push_back({"RPAREN", ")"});ct++;}
            else if (current_char == '{') {tokens.push_back({"LBRACE", "{"});ct++;}
            else if (current_char == '}') {tokens.push_back({"RBRACE", "}"});ct++;}
            else if (current_char == ',') {tokens.push_back({"COMMA", ","});ct++;}
        }
    }

    tpl.push_back(ct); // add the last line token count
    return {tokens, tpl};
}
