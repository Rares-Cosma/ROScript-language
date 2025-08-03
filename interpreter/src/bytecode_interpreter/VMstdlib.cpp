#include "VMstdlib.h"

unordered_map<string, VMBuiltinFunc> VMstdlib = {
    {"afiseaza", [](vector<int32_t>& stack, uint32_t argc) {
        vector<uint32_t> args;
        while (argc>0){
            uint32_t val = stack.back(); 
            stack.pop_back();
            args.push_back(val);
            argc--;
        }
        reverse(args.begin(),args.end());
        for (auto i:args){
            cout<<i;
        }
    }},
    {"e", [](vector<int32_t>& stack, uint32_t argc) {
        exit(0);
    }},
};

vector<string> VMinitBuiltinNames() {
    vector<string> builtinNames;
    for (const auto& kv : VMstdlib) {
        builtinNames.push_back(kv.first);
    }
    return builtinNames;
}