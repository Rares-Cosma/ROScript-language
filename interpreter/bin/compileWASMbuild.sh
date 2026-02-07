#!/bin/bash

# --- List of source files ---
SRC="../src/WASM/wasm_lexer.cpp ../src/stdlib.cpp ../src/parser.cpp ../src/commons.cpp ../src/interpreter.cpp ../src/WASM/wasm_entry.cpp ../src/functionCall.cpp ../src/errors.cpp ../src/ansi.cpp ../src/bytecode_interpreter/compiler.cpp ../src/bytecode_interpreter/vm.cpp ../src/bytecode_interpreter/VMstdlib.cpp"

# --- Compile with em++ ---
em++ $SRC -o roscript.js \
    -s NO_EXIT_RUNTIME=1 \
    -s NO_DISABLE_EXCEPTION_CATCHING \
    -s EXPORTED_RUNTIME_METHODS="['ccall']" \
    -s EXPORTED_FUNCTIONS="['_run_roscript']"
