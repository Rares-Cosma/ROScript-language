#include "modules.h"

class Module {
public:
    ModuleType type;
    std::string name;
    std::vector<std::string> functions;

    Module(ModuleType t, const std::string& n, const std::vector<std::string>& f) : type(t), name(n), functions(f) {}
};