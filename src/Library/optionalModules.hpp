#pragma once
#include "KataScript.hpp"

namespace KataScript {
    void KataScriptInterpreter::createOptionalModules() {
        newModule("file", ModulePrivilege::fileSystemRead | ModulePrivilege::fileSystemWrite, {
            { "saveFile", [](const List& args) {
                if (args.size() == 2
                    && args[0]->type == Type::String
                    && args[1]->type == Type::String
                    ) {
                    auto t = std::ofstream(args[1]->getString(), std::ofstream::out);
                    t << args[0]->getString();
                    t.flush();
                    return make_shared<Value>(true);
                }
                return make_shared<Value>(false);
            }},
            { "readFile", [](const List& args) {
                if (args.size() == 1 && args[0]->type == Type::String) {
                    std::stringstream buffer;
                    buffer << std::ifstream(args[0]->getString()).rdbuf();
                    return make_shared<Value>(buffer.str());
                }
                return make_shared<Value>();
            }},
        });
    }
}
