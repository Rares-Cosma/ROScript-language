#include <iostream>
#include "lexer.h"
#include "errors.h"
#include "ansi.h"
#include "interpreter.h"
using namespace std;

void process(string filename, bool profiler){
	pair<vector<pair<string, string>>,vector<int>> tokens = lexer(filename);

	/*for (const pair<string,string> &p : tokens.first) {
		cout << p.first << " -> " << p.second << endl;
	}*/

	try {
    	interpret(parse(tokens.first,tokens.second),false,profiler,true);
	} catch (const Error& e) {
    	std::cerr << e.what() << std::endl;
    	return;
	} catch (const std::string& e) {
    	std::cerr << "String error: " << e << std::endl;
    	return;
	} catch (const std::exception& e) {
    	std::cerr << "Unexpected internal error: " << e.what() << std::endl;
    	return;
	}
	
}

int main(int argc, char *argv[]){
	enableANSI();
	switch (argc) {
		case 1: cout<<"Nici un fisier specificat. Te rog sa specifici un fisier ROScript.\n"; break;
		case 2: process(argv[1], false); break;
		case 3: {
			if (string(argv[1]) == "-p") {
				process(argv[2], true);
			} else {
				cout<<"Argumente invalide.\n";
			}
			break;
		}
		default: cout<<"Prea multe argumente specificate.\n"; break;
	}
	return 0;
}
