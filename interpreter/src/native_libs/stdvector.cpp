#include "stdvector.h"

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

unordered_map<string, BuiltinFunc> stdvector = {
    {"alipire", [](const vector<Value>& args) {
        if (args.size() != 2)
            throw "concat function expects exactly two arguments";

        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]) ||
            !holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[1]))
            throw "concat expects two list arguments";

        auto vec1 = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
        auto vec2 = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[1]);

        auto result = std::make_shared<std::vector<RecursiveValue>>();
        result->reserve(vec1->size() + vec2->size());
        result->insert(result->end(), vec1->begin(), vec1->end());
        result->insert(result->end(), vec2->begin(), vec2->end());

        return Value{result};
    }},
    {"gaseste", [](const vector<Value>& args) {
        if (args.size() != 2)
            throw "gaseste function expects exactly two arguments";

        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            throw "gaseste expects a list as first argument";

        auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
        const Value& valoare_cautata = args[1];

        for (size_t i = 0; i < vec->size(); i++) {
            if ((*vec)[i] == valoare_cautata) {
                return Value{static_cast<int>(i)};
            }
        }
        return Value{-1};
    }},
    {"sterge", [](const vector<Value>& args) {
        if (args.size() != 2)
            throw "sterge function expects exactly two arguments";

        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            throw "sterge expects a list as first argument";

        auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
        const Value& valoare_de_sters = args[1];

        for (auto it = vec->begin(); it != vec->end(); ) {
            if (*it == valoare_de_sters) {
                it = vec->erase(it);
            } else {
                ++it;
            }
        }
        return Value{0}; // indicate success
    }},
    {"sterge_index", [](const vector<Value>& args) {
        if (args.size() != 2)
            throw "sterge_index function expects exactly two arguments";
        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            throw "sterge_index expects a list as first argument";
        auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
        if (!holds_alternative<int>(args[1]))
            throw "sterge_index expects an int as second argument";
        int index = get<int>(args[1]);
        if (index < 0 || index >= vec->size())
            throw "sterge_index index out of bounds";
        vec->erase(vec->begin() + index);
        return Value{0}; // indicate success
    }},
    {"inserare", [](const vector<Value>& args) {
        if (args.size() != 3)
            throw "insereaza function expects exactly three arguments";

        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            throw "insereaza expects a list as first argument";

        if (!holds_alternative<int>(args[1]))
            throw "insereaza expects an int index as second argument";

        auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
        int idx = get<int>(args[1]);
        const Value& val = args[2];

        if (idx < 0 || static_cast<size_t>(idx) > vec->size())
            throw "insereaza index out of range";

        vec->insert(vec->begin() + idx, val);

        return Value{0};
    }},
    {"inversare", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "inversare function expects exactly one argument";

        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            throw "inversare expects a list argument";

        auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
        std::reverse(vec->begin(), vec->end());

        return Value{0}; // indicate success
    }},
    {"contine", [](const vector<Value>& args) {
        if (args.size() != 2)
            throw "contine function expects exactly two arguments";

        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            throw "contine expects a list as first argument";

        auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
        const Value& valoare_cautata = args[1];

        for (const auto& el : *vec) {
            if (el == valoare_cautata) {
                return Value{true};
            }
        }
        return Value{false};
    }},
    {"contine_index", [](const vector<Value>& args) {
        if (args.size() != 2)
            throw "contine_index function expects exactly two arguments";
        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            throw "contine_index expects a list as first argument";
        auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
        const Value& valoare_cautata = args[1];
        for (size_t i = 0; i < vec->size(); ++i) {
            if ((*vec)[i] == valoare_cautata) {
                return Value{static_cast<int>(i)};
            }
        }
        return Value{-1}; // indicate not found
    }},
    {"sorteaza", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "sorteaza function expects exactly one argument";

        if (!holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]))
            throw "sorteaza expects a list argument";

        auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);

        // Comparator simplu care suportă int, float, string (nu neomogene!)
        auto cmp = [](const RecursiveValue& a, const RecursiveValue& b) {
            if (holds_alternative<int>(a) && holds_alternative<int>(b)) {
                return get<int>(a) < get<int>(b);
            } else if (holds_alternative<float>(a) && holds_alternative<float>(b)) {
                return get<float>(a) < get<float>(b);
            } else if (holds_alternative<int>(a) && holds_alternative<float>(b)) {
                return static_cast<float>(get<int>(a)) < get<float>(b);
            } else if (holds_alternative<float>(a) && holds_alternative<int>(b)) {
                return get<float>(a) < static_cast<float>(get<int>(b));
            } else if (holds_alternative<string>(a) && holds_alternative<string>(b)) {
                return get<string>(a) < get<string>(b);
            } else {
                throw "sorteaza: lista conține elemente de tipuri diferite sau nesuportate pentru sortare";
            }
        };

        std::sort(vec->begin(), vec->end(), cmp);

        return Value{0};
    }},
    {"adauga", [](const vector<Value>& args) {
        if (args.size() != 2) {
            throw "adauga function expects two arguments";
        }
        if (holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(args[0])) {
            auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(args[0]);
            vec->push_back(args[1]);
            return Value{0};
        } else {
            throw "adauga function expects a vector and a Value as arguments";
        }
    }}
};

vector<string> initBuiltinNamesVector() {
    vector<string> builtinNames;
    for (const auto& kv : stdvector) {
        builtinNames.push_back(kv.first);
    }
    return builtinNames;
}