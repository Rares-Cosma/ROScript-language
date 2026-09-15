#include "VMstdfisier.h"

unordered_map<string, VMBuiltinFunc> VMstdfisier = {

    {"citeste_fisier", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1)
            throw std::runtime_error("citeste_fisier function expects exactly one argument");

        VMValue filenameVal = stack.back();
        stack.pop_back();

        if (filenameVal.type != VAL_STRING)
            throw std::runtime_error("citeste_fisier expects a string filename as argument");

        string filename = stringPool[filenameVal.asString];

        std::ifstream file(filename);

        if (!file.is_open())
            throw std::runtime_error("Failed to open file: " + filename);

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        string content = buffer.str();

        uint32_t stringIndex = static_cast<uint32_t>(stringPool.size());
        stringPool.push_back(content);

        stack.push_back(VMValue{VAL_STRING, .asString = stringIndex});
    }},

    {"scrie_fisier", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2)
            throw std::runtime_error("scrie_fisier function expects exactly two arguments");

        VMValue contentVal = stack.back();
        stack.pop_back();

        VMValue filenameVal = stack.back();
        stack.pop_back();

        if (filenameVal.type != VAL_STRING || contentVal.type != VAL_STRING)
            throw std::runtime_error("scrie_fisier expects two string arguments: filename and content");

        string filename = stringPool[filenameVal.asString];
        string content = stringPool[contentVal.asString];

        std::ofstream file(filename);

        if (!file.is_open())
            throw std::runtime_error("Failed to open file for writing: " + filename);

        file << content;
        file.close();

        stack.push_back(VMValue{VAL_INT, .asInt = 0});
    }}

};

vector<string> VMinitBuiltinNamesFisier() {

    vector<string> builtinNames;

    for (const auto& kv : VMstdfisier) {

        builtinNames.push_back(kv.first);
    }

    return builtinNames;

}