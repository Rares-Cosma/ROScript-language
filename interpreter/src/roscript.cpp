#include <iostream>
#include "lexer.h"
#include "interpreter.h"
using namespace std;

void process(string filename, bool profiler){
	pair<vector<pair<string, string>>,vector<int>> tokens = lexer(filename);

	//for (const pair<string,string> &p : tokens) {
	//	cout << p.first << " -> " << p.second << endl;
	//}

	interpret(parse(tokens.first,tokens.second),false,profiler,true);
}

int main(int argc, char *argv[]){
	switch (argc) {
		case 1: cout<<"No file specified in the command.\n"; break;
		case 2: process(argv[1], false); break;
		case 3: {
			if (string(argv[1]) == "-p") {
				process(argv[2], true);
			} else {
				cout<<"Invalid command line arguments.\n";
			}
			break;
		}
		default: cout<<"Too many arguments specified.\n"; break;
	}
	return 0;
}
