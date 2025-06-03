#!/bin/bash

g++ -std=c++17   -Wall -Wextra ../src/lexer.cpp ../src/parser.cpp ../src/commons.cpp ../src/interpreter.cpp ../src/roscript.cpp -o ros

cp ros ../../roscript-ide
echo "Finished compiling."  
