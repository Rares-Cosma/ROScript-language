#include <iostream>
#include <string>
#include "wasm_lexer.h"
#include "../errors.h"
#include "../ansi.h"
#include "../interpreter.h"
#include "../bytecode_interpreter/compiler.h"
#include "../bytecode_interpreter/vm.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Wrapper function for output
inline void print_output(const std::string& msg) {
#ifdef __EMSCRIPTEN__
    EM_ASM({
        let out = document.getElementById('output');
        if (out) out.textContent += UTF8ToString($0) + '\n';
    }, msg.c_str());
#else
    std::cout << msg << std::endl;
#endif
}

extern "C" {
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
int run_roscript(const char* source, int profiler = false, int useBytecode = false) {
    try {
        std::string code(source);

        if (useBytecode) {
            auto tokens = lexer(code); 
            std::string tmpFile = "temp.ros";
            EPCompile(parse(tokens.first, tokens.second), tmpFile);

            VM vm;
            vm.bytecode = loadBytecode(tmpFile + ".rosbc");
            vm.run();
        } else {
            auto tokens = lexer(code);
            interpret(parse(tokens.first, tokens.second), false, profiler, true);
        }
        return 0;
    }
    catch (const Error& e) {
        print_output(e.what());
        return 1;
    }
    catch (const std::string& e) {
        print_output(e);
        return 1;
    }
    catch (const std::exception& e) {
        print_output(e.what());
        return 1;
    }
}

}

/*int main() {
    std::cout << "This is a WebAssembly module. It cannot be run directly.\n";
    run_roscript("afiseaza(\"Salut, lume!\")", 0,0);
    return 0;
}*/