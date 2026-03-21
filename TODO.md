# ROScript VM Argument Population Task

## Steps:

1. [x] Edit `interpreter/src/bytecode_interpreter/opCode.h`: Add `OP_FUNC_ARGS` enum value.

2. [ ] Edit `interpreter/src/bytecode_interpreter/vm.h`: Add `std::vector<std::vector<uint8_t>> functionArgIds;` to VM struct.

3. [ ] Edit `interpreter/src/bytecode_interpreter/compiler.cpp`: Modify FunctionDefinition compilation to collect argIds, emit OP_FDECL, OP_FUNC_ARGS data, adjust JMP placeholder.

4. [ ] Edit `interpreter/src/bytecode_interpreter/vm.cpp`: Handle OP_FUNC_ARGS in run(), update OP_FDECL to parse arg data, implement arg population in OP_CALL.

5. [ ] Compile and test: Run `./interpreter/bin/compile.sh interpreter/bin/main.ros`, then run bytecode, check function args work.

6. [ ] Update this TODO with progress, attempt_completion when done.

