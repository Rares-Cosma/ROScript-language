#include <iostream>
#include "lexer.h"
#include "interpreter.h"
using namespace std;

void process(string filename){
	vector<pair<string, string>> tokens = lexer(filename);

	//for (const pair<string,string> &p : tokens) {
	//	cout << p.first << " -> " << p.second << endl;
	//}

	interpret(parse(tokens));
}

int main(int argc, char *argv[]){
	switch (argc) {
		case 1: cout<<"No file specified in the command.\n"; break;
		case 2: process(argv[1]); break;
		default: cout<<"Too many arguments specified.\n"; break;
	}
	return 0;
}
