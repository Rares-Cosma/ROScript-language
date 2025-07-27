#include "variables.h"
#include <functional>
#include <cmath>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

using BuiltinFunc = function<Value(const vector<Value>&)>;

inline unordered_map<string, BuiltinFunc> stdlib = {
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
    }},
    {"logic", [](const vector<Value>& args) {
        if (args.size() != 1)
            throw "bool function expects a single argument";

        if (holds_alternative<bool>(args[0]))
            return args[0];
        if (holds_alternative<int>(args[0]))
            return Value{get<int>(args[0]) != 0};
        if (holds_alternative<float>(args[0]))
            return Value{get<float>(args[0]) != 0.0f};
        if (holds_alternative<string>(args[0]))
            return Value{!get<string>(args[0]).empty()};

        throw "bool function cannot convert the provided type";
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
    {"media", [](const vector<Value>& args) {
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
    {"concat", [](const vector<Value>& args) {
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
    {"insereaza", [](const vector<Value>& args) {
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
    {"citeste", [](const vector<Value>& args) {
        if (args.size() > 1) {
            throw "citeste function expects a single string argument";
        }
        if (args.size()==0){
            string input;
            getline(cin, input);
            return Value{input}; // return the input as a string
        }
        string prompt = get<string>(args[0]);
        cout << prompt;
        string input;
        getline(cin, input);
        return Value{input}; // return the input as a string
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
    }},
    {"afiseaza", [](const vector<Value>& args) {
        for (auto& arg : args) {
            if (holds_alternative<int>(arg)) {
                cout << get<int>(arg);
            } else if (holds_alternative<float>(arg)) {
                cout << get<float>(arg);
            } else if (holds_alternative<string>(arg)) {
                cout << get<string>(arg);
            } else if (holds_alternative<bool>(arg)) {
                cout << (get<bool>(arg) ? "adevarat" : "fals");
            } else if (holds_alternative<std::shared_ptr<std::vector<RecursiveValue>>>(arg)) {
                auto vec = get<std::shared_ptr<std::vector<RecursiveValue>>>(arg);
                cout << "[";
                for (size_t i = 0; i < vec->size(); ++i)
                {
                    if (holds_alternative<int>((*vec)[i])) {
                        cout << get<int>((*vec)[i]);
                    } else if (holds_alternative<float>((*vec)[i])) {
                        cout << get<float>((*vec)[i]);
                    } else if (holds_alternative<string>((*vec)[i])) {
                        cout << get<string>((*vec)[i]);
                    } else if (holds_alternative<bool>((*vec)[i])) {
                        cout << (get<bool>((*vec)[i]) ? "adevarat" : "fals");
                    } else {
                        throw "afiseaza function cannot handle the provided type in vector";
                    }
                    if (i < vec->size() - 1) {
                        cout << ", ";
                    }
                }
                cout << "]";
            } else {
                throw "afiseaza function cannot handle the provided type";
            }
        }
        return Value{0}; // indicate success
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