#include "VMstdmatematica.h"

unordered_map<string, VMBuiltinFunc> VMstdmatematica = {

    {"minim", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2) throw std::runtime_error("minim function expects exactly two arguments");

        VMValue b = stack.back();
        stack.pop_back();

        VMValue a = stack.back();
        stack.pop_back();

        if ((a.type != VAL_INT && a.type != VAL_FLOAT) || (b.type != VAL_INT && b.type != VAL_FLOAT))
            throw std::runtime_error("minim expects numeric arguments");

        double aValue = a.type == VAL_INT ? static_cast<double>(a.asInt) : a.asFloat;
        double bValue = b.type == VAL_INT ? static_cast<double>(b.asInt) : b.asFloat;
        double result = std::min(aValue, bValue);

        if (a.type == VAL_INT && b.type == VAL_INT)
            stack.push_back(VMValue{VAL_INT, .asInt = static_cast<int32_t>(result)});
        else
            stack.push_back(VMValue{VAL_FLOAT, .asFloat = result});
    }},

    {"maxim", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2) throw std::runtime_error("maxim function expects exactly two arguments");

        VMValue b = stack.back();
        stack.pop_back();

        VMValue a = stack.back();
        stack.pop_back();

        if ((a.type != VAL_INT && a.type != VAL_FLOAT) || (b.type != VAL_INT && b.type != VAL_FLOAT))
            throw std::runtime_error("maxim expects numeric arguments");

        double aValue = a.type == VAL_INT ? static_cast<double>(a.asInt) : a.asFloat;
        double bValue = b.type == VAL_INT ? static_cast<double>(b.asInt) : b.asFloat;
        double result = std::max(aValue, bValue);

        if (a.type == VAL_INT && b.type == VAL_INT)
            stack.push_back(VMValue{VAL_INT, .asInt = static_cast<int32_t>(result)});
        else
            stack.push_back(VMValue{VAL_FLOAT, .asFloat = result});
    }},

    {"abs", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) throw std::runtime_error("abs function expects exactly one argument");

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_INT) {

            stack.push_back(VMValue{VAL_INT, .asInt = val.asInt < 0 ? -val.asInt : val.asInt});

        } else if (val.type == VAL_FLOAT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = std::fabs(val.asFloat)});

        } else {

            throw std::runtime_error("abs expects numeric argument");
        }
    }},

    {"putere", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 2) throw std::runtime_error("putere function expects exactly two arguments");

        VMValue exp = stack.back();
        stack.pop_back();

        VMValue base = stack.back();
        stack.pop_back();

        if (base.type == VAL_INT && exp.type == VAL_INT) {

            stack.push_back(VMValue{VAL_INT, .asInt = static_cast<int32_t>(std::pow(base.asInt, exp.asInt))});

        } else if (base.type == VAL_FLOAT && exp.type == VAL_FLOAT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = std::pow(base.asFloat, exp.asFloat)});

        } else {

            throw std::runtime_error("putere function expects numeric arguments");
        }
    }},

    {"pi", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 0) throw std::runtime_error("pi function does not take any arguments");

        stack.push_back(VMValue{VAL_FLOAT, .asFloat = 3.14159265358979323846f});
    }},

    {"cos", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) throw std::runtime_error("cos function expects a single argument");

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_INT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = cos(static_cast<double>(val.asInt))});

        } else if (val.type == VAL_FLOAT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = cos(val.asFloat)});

        } else {

            throw std::runtime_error("cos function expects an int or float argument");
        }
    }},

    {"sin", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) throw std::runtime_error("sin function expects a single argument");

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_INT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = sin(static_cast<double>(val.asInt))});

        } else if (val.type == VAL_FLOAT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = sin(val.asFloat)});

        } else {

            throw std::runtime_error("sin function expects an int or float argument");
        }
    }},

    {"tan", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) throw std::runtime_error("tan function expects a single argument");

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_INT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = tan(static_cast<double>(val.asInt))});

        } else if (val.type == VAL_FLOAT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = tan(val.asFloat)});

        } else {

            throw std::runtime_error("tan function expects an int or float argument");
        }
    }},

    {"log", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) throw std::runtime_error("log function expects a single argument");

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_INT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = log(static_cast<double>(val.asInt))});

        } else if (val.type == VAL_FLOAT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = log(val.asFloat)});

        } else {

            throw std::runtime_error("log function expects an int or float argument");
        }
    }},

    {"factorial", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) throw std::runtime_error("factorial function expects a single argument");

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_INT) {

            int32_t n = val.asInt;

            if (n < 0) throw std::runtime_error("factorial function does not accept negative numbers");

            int32_t result = 1;

            for (int32_t i = 2; i <= n; ++i) {

                result *= i;
            }

            stack.push_back(VMValue{VAL_INT, .asInt = result});

        } else {

            throw std::runtime_error("factorial function expects an int argument");
        }
    }},

    {"suma", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) throw std::runtime_error("suma function expects exactly one argument");

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type != VAL_LIST)
            throw std::runtime_error("suma expects a list argument");

        auto& vec = listPool[val.asList];

        double sum = 0.0;

        for (auto& el : vec) {

            if (el.type == VAL_INT) {

                sum += static_cast<double>(el.asInt);

            } else if (el.type == VAL_FLOAT) {

                sum += el.asFloat;

            } else {

                throw std::runtime_error("suma only supports lists of numbers");
            }
        }

        stack.push_back(VMValue{VAL_FLOAT, .asFloat = sum});
    }},

    {"medie", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) throw std::runtime_error("media function expects exactly one argument");

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type != VAL_LIST)
            throw std::runtime_error("media expects a list argument");

        auto& vec = listPool[val.asList];

        double sum = 0.0;

        for (auto& el : vec) {

            if (el.type == VAL_INT) {

                sum += static_cast<double>(el.asInt);

            } else if (el.type == VAL_FLOAT) {

                sum += el.asFloat;

            } else {

                throw std::runtime_error("media only supports lists of numbers");
            }
        }

        stack.push_back(VMValue{VAL_FLOAT, .asFloat = sum / vec.size()});
    }},

    {"radp", [](vector<VMValue>& stack, uint32_t argc, vector<string>& stringPool, vector<vector<VMValue>>& listPool) {

        if (argc != 1) throw std::runtime_error("sqrt function expects a single argument");

        VMValue val = stack.back();
        stack.pop_back();

        if (val.type == VAL_INT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = sqrt(static_cast<double>(val.asInt))});

        } else if (val.type == VAL_FLOAT) {

            stack.push_back(VMValue{VAL_FLOAT, .asFloat = sqrt(val.asFloat)});

        } else {

            throw std::runtime_error("sqrt function expects an int or float argument");
        }
    }}

};

vector<string> VMinitBuiltinNamesMatematica() {

    vector<string> builtinNames;

    for (const auto& kv : VMstdmatematica) {

        builtinNames.push_back(kv.first);
    }

    return builtinNames;

}