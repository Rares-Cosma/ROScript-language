#include "VMstdvector.h"

static bool vmValueEquals(const VMValue& a, const VMValue& b, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

    if (a.type != b.type) return false;

    if (a.type == VAL_INT) return a.asInt == b.asInt;

    if (a.type == VAL_FLOAT) return a.asFloat == b.asFloat;

    if (a.type == VAL_BOOL) return a.asBool == b.asBool;

    if (a.type == VAL_STRING) return stringPool[a.asString] == stringPool[b.asString];

    if (a.type == VAL_LIST) {

        auto& list1 = listPool[a.asList];
        auto& list2 = listPool[b.asList];

        if (list1.size() != list2.size()) return false;

        for (size_t i = 0; i < list1.size(); ++i) {

            if (!vmValueEquals(list1[i], list2[i], stringPool, listPool))
                return false;
        }

        return true;
    }

    return false;
}

unordered_map<string, VMBuiltinFunc> VMstdvector = {

    {"alipire", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2)
            throw std::runtime_error("concat function expects exactly two arguments");

        VMValue vec2 = stack.back();
        stack.pop_back();

        VMValue vec1 = stack.back();
        stack.pop_back();

        if (vec1.type != VAL_LIST || vec2.type != VAL_LIST)
            throw std::runtime_error("concat expects two list arguments");

        auto& list1 = listPool[vec1.asList];
        auto& list2 = listPool[vec2.asList];

        auto result = vector<VMValue>();

        result.reserve(list1.size() + list2.size());
        result.insert(result.end(), list1.begin(), list1.end());
        result.insert(result.end(), list2.begin(), list2.end());

        uint32_t listIndex = static_cast<uint32_t>(listPool.size());
        listPool.push_back(result);

        stack.push_back(VMValue{VAL_LIST, .asList = listIndex});
    }},

    {"gaseste", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2)
            throw std::runtime_error("gaseste function expects exactly two arguments");

        VMValue valoare_cautata = stack.back();
        stack.pop_back();

        VMValue listVal = stack.back();
        stack.pop_back();

        if (listVal.type != VAL_LIST)
            throw std::runtime_error("gaseste expects a list as first argument");

        auto& vec = listPool[listVal.asList];

        for (size_t i = 0; i < vec.size(); i++) {

            if (vmValueEquals(vec[i], valoare_cautata, stringPool, listPool)) {

                stack.push_back(VMValue{VAL_INT, .asInt = static_cast<int32_t>(i)});
                return;
            }
        }

        stack.push_back(VMValue{VAL_INT, .asInt = -1});
    }},

    {"sterge", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2)
            throw std::runtime_error("sterge function expects exactly two arguments");

        VMValue valoare_de_sters = stack.back();
        stack.pop_back();

        VMValue listVal = stack.back();
        stack.pop_back();

        if (listVal.type != VAL_LIST)
            throw std::runtime_error("sterge expects a list as first argument");

        auto& vec = listPool[listVal.asList];

        for (auto it = vec.begin(); it != vec.end(); ) {

            if (vmValueEquals(*it, valoare_de_sters, stringPool, listPool)) {

                it = vec.erase(it);

            } else {

                ++it;
            }
        }

        stack.push_back(VMValue{VAL_INT, .asInt = 0});
    }},

    {"sterge_index", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2)
            throw std::runtime_error("sterge_index function expects exactly two arguments");

        VMValue indexVal = stack.back();
        stack.pop_back();

        VMValue listVal = stack.back();
        stack.pop_back();

        if (listVal.type != VAL_LIST)
            throw std::runtime_error("sterge_index expects a list as first argument");

        if (indexVal.type != VAL_INT)
            throw std::runtime_error("sterge_index expects an int as second argument");

        auto& vec = listPool[listVal.asList];

        int32_t index = indexVal.asInt;

        if (index < 0 || index >= vec.size())
            throw std::runtime_error("sterge_index index out of bounds");

        vec.erase(vec.begin() + index);

        stack.push_back(VMValue{VAL_INT, .asInt = 0});
    }},

    {"inserare", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 3)
            throw std::runtime_error("insereaza function expects exactly three arguments");

        VMValue val = stack.back();
        stack.pop_back();

        VMValue indexVal = stack.back();
        stack.pop_back();

        VMValue listVal = stack.back();
        stack.pop_back();

        if (listVal.type != VAL_LIST)
            throw std::runtime_error("insereaza expects a list as first argument");

        if (indexVal.type != VAL_INT)
            throw std::runtime_error("insereaza expects an int index as second argument");

        auto& vec = listPool[listVal.asList];

        int32_t idx = indexVal.asInt;

        if (idx < 0 || static_cast<size_t>(idx) > vec.size())
            throw std::runtime_error("insereaza index out of range");

        vec.insert(vec.begin() + idx, val);

        stack.push_back(VMValue{VAL_INT, .asInt = 0});
    }},

    {"inversare", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1)
            throw std::runtime_error("inversare function expects exactly one argument");

        VMValue listVal = stack.back();
        stack.pop_back();

        if (listVal.type != VAL_LIST)
            throw std::runtime_error("inversare expects a list argument");

        auto& vec = listPool[listVal.asList];

        std::reverse(vec.begin(), vec.end());

        stack.push_back(VMValue{VAL_INT, .asInt = 0});
    }},

    {"contine", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2)
            throw std::runtime_error("contine function expects exactly two arguments");

        VMValue valoare_cautata = stack.back();
        stack.pop_back();

        VMValue listVal = stack.back();
        stack.pop_back();

        if (listVal.type != VAL_LIST)
            throw std::runtime_error("contine expects a list as first argument");

        auto& vec = listPool[listVal.asList];

        for (const auto& el : vec) {

            if (vmValueEquals(el, valoare_cautata, stringPool, listPool)) {

                stack.push_back(VMValue{VAL_BOOL, .asBool = true});
                return;
            }
        }

        stack.push_back(VMValue{VAL_BOOL, .asBool = false});
    }},

    {"contine_index", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2)
            throw std::runtime_error("contine_index function expects exactly two arguments");

        VMValue valoare_cautata = stack.back();
        stack.pop_back();

        VMValue listVal = stack.back();
        stack.pop_back();

        if (listVal.type != VAL_LIST)
            throw std::runtime_error("contine_index expects a list as first argument");

        auto& vec = listPool[listVal.asList];

        for (size_t i = 0; i < vec.size(); ++i) {

            if (vmValueEquals(vec[i], valoare_cautata, stringPool, listPool)) {

                stack.push_back(VMValue{VAL_INT, .asInt = static_cast<int32_t>(i)});
                return;
            }
        }

        stack.push_back(VMValue{VAL_INT, .asInt = -1});
    }},

    {"sorteaza", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1)
            throw std::runtime_error("sorteaza function expects exactly one argument");

        VMValue listVal = stack.back();
        stack.pop_back();

        if (listVal.type != VAL_LIST)
            throw std::runtime_error("sorteaza expects a list argument");

        auto& vec = listPool[listVal.asList];

        auto cmp = [&stringPool](const VMValue& a, const VMValue& b) {

            if (a.type == VAL_INT && b.type == VAL_INT) {

                return a.asInt < b.asInt;

            } else if (a.type == VAL_FLOAT && b.type == VAL_FLOAT) {

                return a.asFloat < b.asFloat;

            } else if (a.type == VAL_INT && b.type == VAL_FLOAT) {

                return static_cast<double>(a.asInt) < b.asFloat;

            } else if (a.type == VAL_FLOAT && b.type == VAL_INT) {

                return a.asFloat < static_cast<double>(b.asInt);

            } else if (a.type == VAL_STRING && b.type == VAL_STRING) {

                return stringPool[a.asString] < stringPool[b.asString];

            } else {

                throw std::runtime_error("sorteaza: lista conține elemente de tipuri diferite sau nesuportate pentru sortare");
            }
        };

        std::sort(vec.begin(), vec.end(), cmp);

        stack.push_back(VMValue{VAL_INT, .asInt = 0});
    }},

    {"adauga", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2) {

            throw std::runtime_error("adauga function expects two arguments");
        }

        VMValue value = stack.back();
        stack.pop_back();

        VMValue listVal = stack.back();
        stack.pop_back();

        if (listVal.type == VAL_LIST) {

            auto& vec = listPool[listVal.asList];

            vec.push_back(value);

            stack.push_back(VMValue{VAL_INT, .asInt = 0});

        } else {

            throw std::runtime_error("adauga function expects a vector and a Value as arguments");
        }
    }}

};

vector<string> VMinitBuiltinNamesVector() {

    vector<string> builtinNames;

    for (const auto& kv : VMstdvector) {

        builtinNames.push_back(kv.first);
    }

    return builtinNames;

}