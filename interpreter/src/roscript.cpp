#include <iostream>
#include "lexer.h"
#include "commons.cpp"
#include "errors.h"
#include "ansi.h"
#include "interpreter.h"
#include "bytecode_interpreter/compiler.h"
#include "bytecode_interpreter/vm.h"
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

int main(int argc, char *argv[]) {
    enableANSI(true);
    codes=true; //put codes, and enable ansi based on os

    if (argc < 2) {
        cout << "Niciun fisier specificat. Te rog sa specifici un fisier ROScript.\n";
        return 1;
    }

    bool profiler = false;
    bool useBytecode = false;
	bool run = false;
    string filename;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-p") {
            profiler = true;
        } else if (arg == "-bc") {
            useBytecode = true;
		} else if (arg == "-r") {
            run = true;
        } else if (arg.rfind("-", 0) == 0) {
            cout << "Argument invalid: " << arg << "\n";
            return 1;
        } else {
            filename = arg;
        }
    }

    if (filename.empty()) {
        cout << "Nu ai specificat niciun fisier ROScript.\n";
        return 1;
    }

    if (useBytecode) {
		pair<vector<pair<string, string>>,vector<int>> tokens = lexer(filename);
		EPCompile(parse(tokens.first,tokens.second),filename);
		if (run) {
			VM vm;
			vm.bytecode=loadBytecode(filename+"bc");
			vm.run();
			return 0;
		}
    } else if (run) {
        if (hasEnding(filename,".rosbc")) {
            VM vm;
			vm.bytecode=loadBytecode(filename);
			vm.run();
			return 0;
        } else if (hasEnding(filename,".ros")) {
            process(filename, profiler);
        }
    } else {
        cout<<"Nu ai specificat nici un steag, precum -bc (compilare), -r (rulare).\n";
    }

    return 0;
}

