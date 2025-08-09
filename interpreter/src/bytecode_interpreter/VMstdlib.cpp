#include "VMstdlib.h"

unordered_map<string, VMBuiltinFunc> VMstdlib = {
    {"afiseaza", [](vector<VMValue>& stack, uint32_t argc, vector<string> stringPool) {
        vector<VMValue> args;
        while (argc>0){
            VMValue val = stack.back(); 
            stack.pop_back();
            args.push_back(val);
            argc--;
        }
        reverse(args.begin(),args.end());
        for (auto i:args){
            if (i.type==VAL_INT){
                cout<<i.asInt;
            } else if (i.type==VAL_FLOAT){
                cout<<i.asFloat;
            } else if (i.type==VAL_STRING){
                cout<<stringPool[i.asString];
            }
        }
    }},
    {"oprire", [](vector<VMValue>& stack, uint32_t argc, vector<string> stringPool) {
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