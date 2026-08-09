#include "stdmatematica.h"

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

unordered_map<string, BuiltinFunc> stdmatematica = {
    {"minim", [](const vector<Value>& args) {
        if (args.size() != 2)
            throw "minim function expects exactly two arguments";

        // funcție helper să convertească la float
        auto to_float = [](const Value& v) -> float {
            if (holds_alternative<int>(v)) return static_cast<float>(get<int>(v));
            if (holds_alternative<float>(v)) return get<float>(v);
            throw "minim expects numeric arguments";
        };

        float a = to_float(args[0]);
        float b = to_float(args[1]);

        float result = std::min(a, b);

        // Dacă ambii erau int, întoarce int, altfel float
        if (holds_alternative<int>(args[0]) && holds_alternative<int>(args[1]))
            return Value{static_cast<int>(result)};
        return Value{result};
    }},
    {"maxim", [](const vector<Value>& args) {
        if (args.size() != 2)
            throw "maxim function expects exactly two arguments";

        // funcție helper să convertească la float
        auto to_float = [](const Value& v) -> float {
            if (holds_alternative<int>(v)) return static_cast<float>(get<int>(v));
            if (holds_alternative<float>(v)) return get<float>(v);
            throw "maxim expects numeric arguments";
        };

        float a = to_float(args[0]);
        float b = to_float(args[1]);

        float result = std::max(a, b);

        // Dacă ambii erau int, întoarce int, altfel float
        if (holds_alternative<int>(args[0]) && holds_alternative<int>(args[1]))
            return Value{static_cast<int>(result)};
        return Value{result};
    }},
    {"abs", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "abs function expects exactly one argument";

        if (holds_alternative<int>(args[0])) {
            int v = get<int>(args[0]);
            return Value{v < 0 ? -v : v};
        }
        if (holds_alternative<float>(args[0])) {
            float v = get<float>(args[0]);
            return Value{std::fabs(v)};
        }

        throw "abs expects numeric argument";
    }},
    {"putere", [](const vector<Value>& args) {
        if (args.size() != 2)
            throw "putere function expects exactly two arguments";

        if (holds_alternative<int>(args[0]) && holds_alternative<int>(args[1])) {
            int base = get<int>(args[0]);
            int exp = get<int>(args[1]);
            return Value{static_cast<int>(std::pow(base, exp))};
        } else if (holds_alternative<float>(args[0]) && holds_alternative<float>(args[1])) {
            float base = get<float>(args[0]);
            float exp = get<float>(args[1]);
            return Value{std::pow(base, exp)};
        } else {
            throw "putere function expects numeric arguments";
        }
    }},
    {"pi", [](const vector<Value>& args) {
        if (!args.empty())
            throw "pi function does not take any arguments";
        return Value{3.14159265358979323846f}; // return pi as a float
    }},
    {"cos", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "cos function expects a single argument";
        if (holds_alternative<int>(args[0])) {
            return Value{cos(static_cast<float>(get<int>(args[0])))};
        } else if (holds_alternative<float>(args[0])) {
            return Value{cos(get<float>(args[0]))};
        } else {
            throw "cos function expects an int or float argument";
        }
    }},
    {"sin", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "sin function expects a single argument";
        if (holds_alternative<int>(args[0])) {
            return Value{sin(static_cast<float>(get<int>(args[0])))};
        } else if (holds_alternative<float>(args[0])) {
            return Value{sin(get<float>(args[0]))};
        } else {
            throw "sin function expects an int or float argument";
        }
    }},
    {"tan", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "tan function expects a single argument";
        if (holds_alternative<int>(args[0])) {
            return Value{tan(static_cast<float>(get<int>(args[0])))};
        } else if (holds_alternative<float>(args[0])) {
            return Value{tan(get<float>(args[0]))};
        } else {
            throw "tan function expects an int or float argument";
        }
    }},
    {"log", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "log function expects a single argument";
        if (holds_alternative<int>(args[0])) {
            return Value{log(static_cast<float>(get<int>(args[0])))};
        } else if (holds_alternative<float>(args[0])) {
            return Value{log(get<float>(args[0]))};
        } else {
            throw "log function expects an int or float argument";
        }
    }},
    {"factorial", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "factorial function expects a single argument";
        if (holds_alternative<int>(args[0])) {
            int n = get<int>(args[0]);
            if (n < 0) throw "factorial function does not accept negative numbers";
            int result = 1;
            for (int i = 2; i <= n; ++i) {
                result *= i;
            }
            return Value{result};
        } else {
            throw "factorial function expects an int argument";
        }
    }},
    {"suma", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "suma function expects exactly one argument";

        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            throw "suma expects a list argument";

        auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
        float sum = 0.0f;

        for (auto& el : *vec) {
            if (holds_alternative<int>(el)) {
                sum += static_cast<float>(get<int>(el));
            } else if (holds_alternative<float>(el)) {
                sum += get<float>(el);
            } else {
                throw "suma only supports lists of numbers";
            }
        }

        // dacă toate au fost int, poți întoarce int
        // (pentru simplitate, întoarce float)
        return Value{sum};
    }},
    {"medie", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "media function expects exactly one argument";

        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            throw "media expects a list argument";

        auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
        float sum = 0.0f;

        for (auto& el : *vec) {
            if (holds_alternative<int>(el)) {
                sum += static_cast<float>(get<int>(el));
            } else if (holds_alternative<float>(el)) {
                sum += get<float>(el);
            } else {
                throw "media only supports lists of numbers";
            }
        }

        return Value{sum / vec->size()};
    }},
    {"radp", [](const vector<Value>& args) {
        if (args.size() != 1) {
            throw "sqrt function expects a single argument";
        }
        if (holds_alternative<int>(args[0])) {
            return Value{sqrt(static_cast<float>(get<int>(args[0])))};
        } else if (holds_alternative<float>(args[0])) {
            return Value{sqrt(get<float>(args[0]))};
        } else {
            throw "sqrt function expects an int or float argument";
        }
    }}
};

vector<string> initBuiltinNamesMatematica() {
    vector<string> builtinNames;
    for (const auto& kv : stdmatematica) {
        builtinNames.push_back(kv.first);
    }
    return builtinNames;
}