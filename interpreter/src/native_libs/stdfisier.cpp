#include "stdfisier.h"

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

inline std::string get_input(const std::string& promptMsg = "") {
#ifdef __EMSCRIPTEN__
    char buffer[1024];
    int len = EM_ASM_INT({
        var input = prompt("");      // single blocking prompt, no message
        if (input === null) return -1; // user hit Cancel
        stringToUTF8(input, $0, 1024);
        return input.length;
    }, buffer);

    if (len == -1) return ""; // user canceled
    return std::string(buffer, len);
#else
    std::string input;
    std::getline(std::cin, input);  // native blocking input
    return input;
#endif
}

unordered_map<string, BuiltinFunc> stdfisier = {
    {"citeste_fisier", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "citeste_fisier function expects exactly one argument";

        if (!holds_alternative<string>(args[0]))
            throw "citeste_fisier expects a string filename as argument";

        string filename = get<string>(args[0]);
        std::ifstream file(filename);

        if (!file.is_open())
            throw "Failed to open file: " + filename;

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        return Value{buffer.str()};
    }},
    {"scrie_fisier", [](const vector<Value>& args) {
        if (args.size() != 2)
            throw "scrie_fisier function expects exactly two arguments";

        if (!holds_alternative<string>(args[0]) || !holds_alternative<string>(args[1]))
            throw "scrie_fisier expects two string arguments: filename and content";

        string filename = get<string>(args[0]);
        string content = get<string>(args[1]);

        std::ofstream file(filename);

        if (!file.is_open())
            throw "Failed to open file for writing: " + filename;

        file << content;
        file.close();

        return Value{0};  // indicate success
    }}
};

vector<string> initBuiltinNamesFisier() {
    vector<string> builtinNames;
    for (const auto& kv : stdfisier) {
        builtinNames.push_back(kv.first);
    }
    return builtinNames;
}