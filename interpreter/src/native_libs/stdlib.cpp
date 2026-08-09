#include "stdlib.h"

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

unordered_map<string, BuiltinFunc> stdlib = {
    {"intreg", [](const vector<Value>& args) {
        if (args.size() != 1) {
            throw "int function expects a single argument";
        }
        if (holds_alternative<int>(args[0])) {
            return args[0]; // already a Value holding int
        } else if (holds_alternative<float>(args[0])) {
            return Value{static_cast<int>(round(get<float>(args[0])))};
        } else if (holds_alternative<string>(args[0])) {
            return Value{stoi(get<string>(args[0]))};
        } else if (holds_alternative<bool>(args[0])) {
            return Value{get<bool>(args[0]) ? 1 : 0};
        } else {
            throw "int function cannot convert the provided type";
        }
    }},
    {"real", [](const vector<Value>& args) {
        if (args.size() != 1) {
            throw "float function expects a single argument";
        }
        if (holds_alternative<float>(args[0])) {
            return args[0]; // already a Value holding float
        } else if (holds_alternative<int>(args[0])) {
            return Value{static_cast<float>(get<int>(args[0]))};
        } else if (holds_alternative<string>(args[0])) {
            return Value{stof(get<string>(args[0]))};
        } else if (holds_alternative<bool>(args[0])) {
            return Value{get<bool>(args[0]) ? 1.0f : 0.0f};
        } else {
            throw "float function cannot convert the provided type";
        }
    }},
    {"oprire", [](const vector<Value>& args) {
        exit(0);
        return Value{0};
    }},
    {"lista", [](const vector<Value>& args) {
        if (args.size() != 1) {
            throw "list function expects a single argument";
        }
        if (holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0])) {
            return args[0]; // already a Value holding list
        } else if (holds_alternative<int>(args[0]) || holds_alternative<float>(args[0]) || holds_alternative<bool>(args[0])) {
            auto vec = make_shared<vector<RecursiveValue>>();
            vec->push_back(args[0]);
            return Value{vec};
        } else if (holds_alternative<string>(args[0])) {
            auto vec = make_shared<vector<RecursiveValue>>();
            for (char c : get<string>(args[0])) {
                vec->push_back(Value{string(1, c)});
            }
            return Value{vec};
        } else {
            throw "list function cannot convert the provided type";
        }
    }},
    {"logic", [](const vector<Value>& args) {
        if (args.size() != 1) {
            throw "bool function expects a single argument";
        }
        if (holds_alternative<bool>(args[0])) {
            return args[0];
        } else if (holds_alternative<int>(args[0])) {
            return Value{get<int>(args[0]) != 0};
        } else if (holds_alternative<float>(args[0])) {
            return Value{get<float>(args[0]) != 0.0f};
        } else if (holds_alternative<string>(args[0])) {
            return Value{!get<string>(args[0]).empty()};
        } else {
            throw "bool function cannot convert the provided type";
        }
    }},
    {"sirc", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "string function expects a single argument";

        if (holds_alternative<string>(args[0]))
            return args[0];
        if (holds_alternative<int>(args[0]))
            return Value{to_string(get<int>(args[0]))};
        if (holds_alternative<float>(args[0]))
            return Value{to_string(get<float>(args[0]))};
        if (holds_alternative<bool>(args[0]))
            return Value{get<bool>(args[0]) ? "adevarat" : "fals"};
        if (holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0])) {
            auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
            std::string result;
            for (const auto& el : *vec) {
                if (holds_alternative<string>(el)) {
                    result += get<string>(el);
                } else if (holds_alternative<int>(el)) {
                    result += to_string(get<int>(el));
                } else if (holds_alternative<float>(el)) {
                    result += to_string(get<float>(el));
                } else {
                    throw "string function expects a list of strings or numbers";
                }
            }
            return Value{result};
        }

        throw "string function cannot convert the provided type";
    }},
    {"lungime", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "len function expects a single argument";

        if (holds_alternative<string>(args[0]))
            return Value{static_cast<int>(get<string>(args[0]).length())};
        else if (holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0])) 
            return Value{static_cast<int>(get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0])->size())};
        else throw "len function expects a string argument";

        throw "len function cannot convert the provided type";
    }},
    {"tip", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "type function expects a single argument";

        if (holds_alternative<int>(args[0]))
            return Value{"intreg"};
        if (holds_alternative<float>(args[0]))
            return Value{"real"};
        if (holds_alternative<string>(args[0]))
            return Value{"sirc"};
        if (holds_alternative<bool>(args[0]))
            return Value{"logic"};
        if (holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            return Value{"lista"};
        else return Value{"necunoscut"};

        throw "type function cannot determine the type of the provided value";
    }},
    {"citeste", [](const vector<Value>& args) {
        if (args.size() > 1) {
            throw "citeste function expects a single string argument";
        }
        if (args.size()==0){
            string input=get_input();
            //getline(cin, input);
            return Value{input}; // return the input as a string
        }
        string prompt = get<string>(args[0]);
        print_output(prompt);
        string input=get_input();
        //getline(cin, input);
        return Value{input}; // return the input as a string
    }},
    {"afiseaza", [](const vector<Value>& args) {
        std::string output;

        for (auto& arg : args) {
            if (holds_alternative<int>(arg)) {
                output += std::to_string(get<int>(arg));
            } else if (holds_alternative<float>(arg)) {
                output += std::to_string(get<float>(arg));
            } else if (holds_alternative<string>(arg)) {
                output += get<string>(arg);
            } else if (holds_alternative<bool>(arg)) {
                output += (get<bool>(arg) ? "adevarat" : "fals");
            } else if (holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(arg)) {
                auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(arg);
                output += "[";
                for (size_t i = 0; i < vec->size(); ++i) {
                    if (holds_alternative<int>((*vec)[i])) {
                        output += std::to_string(get<int>((*vec)[i]));
                    } else if (holds_alternative<float>((*vec)[i])) {
                        output += std::to_string(get<float>((*vec)[i]));
                    } else if (holds_alternative<string>((*vec)[i])) {
                        output += get<string>((*vec)[i]);
                    } else if (holds_alternative<bool>((*vec)[i])) {
                        output += (get<bool>((*vec)[i]) ? "adevarat" : "fals");
                    } else {
                        throw "afiseaza function cannot handle the provided type in vector";
                    }
                    if (i < vec->size() - 1) {
                        output += ", ";
                    }
                }
                output += "]";
            } else {
                throw "afiseaza function cannot handle the provided type";
            }
        }

        print_output(output);

        return Value{0}; // indicate success
    }}
};

vector<string> initBuiltinNamesStdlib() {
    vector<string> builtinNames;
    for (const auto& kv : stdlib) {
        builtinNames.push_back(kv.first);
    }
    return builtinNames;
}