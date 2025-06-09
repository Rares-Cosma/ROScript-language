#!/bin/bash

SRC="../src/lexer.cpp ../src/stdlib.cpp ../src/parser.cpp ../src/commons.cpp ../src/interpreter.cpp ../src/roscript.cpp"
OUT="ros"
OBJDIR="./obj"
WARNFILE="warnings.log"

mkdir -p $OBJDIR
# Clear warnings file at start
> $WARNFILE

echo "Compiling sources..."

for srcfile in $SRC; do
    objfile="$OBJDIR/$(basename ${srcfile%.*}.o)"
    echo "Compiling $srcfile ..."

    # Compile without forcing colors for warnings
    g++ -std=c++17 -Wall -Wextra -g -c "$srcfile" -o "$objfile" 2> tmp_stderr.log

    # Strip colors
    sed -r "s/\x1B\[[0-9;]*[mK]//g" tmp_stderr.log > tmp_stderr_nocolor.log

    if grep -q "error:" tmp_stderr_nocolor.log; then
        # Print colored errors from original file
        grep --color=always "error:" tmp_stderr.log >&2
        echo "Compilation failed on $srcfile"
        rm tmp_stderr.log tmp_stderr_nocolor.log
        exit 1
    else
        # Append warnings without colors
        grep -v "error:" tmp_stderr_nocolor.log >> $WARNFILE
        rm tmp_stderr.log tmp_stderr_nocolor.log
    fi
done

echo "Linking..."

# Link all object files, redirect stderr similarly
g++ -std=c++17 -Wall -Wextra -fdiagnostics-color=always -g $OBJDIR/*.o -o $OUT 2> tmp_stderr.log

if [ -s tmp_stderr.log ]; then
    echo "Linker output:"
    cat tmp_stderr.log >&2
    echo "Linking failed."
    rm tmp_stderr.log
    exit 1
else
    rm tmp_stderr.log
fi

# Copy executable for IDE
if [ -f "$OUT" ]; then
    cp "$OUT" ../../roscript-ide
    echo "Finished compiling and copied executable."
else
    echo "Executable not found after linking."
    exit 1
fi
