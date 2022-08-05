#pragma once
#include "KataScript.hpp"

namespace KataScript {
    void KataScriptInterpreter::createOptionalModules() {
        newModule("file", ModulePrivilege::fileSystemRead | ModulePrivilege::fileSystemWrite, {
            { "saveFile", [](const KSList& args) {
                if (args.size() == 2
                    && args[0]->type == KSType::String
                    && args[1]->type == KSType::String
                    ) {
                    auto t = std::ofstream(args[1]->getString(), std::ofstream::out);
                    t << args[0]->getString();
                    t.flush();
                    return make_shared<KSValue>(true);
                }
                return make_shared<KSValue>(false);
            }},
            { "readFile", [](const KSList& args) {
                if (args.size() == 1 && args[0]->type == KSType::String) {
                    std::stringstream buffer;
                    buffer << std::ifstream(args[0]->getString()).rdbuf();
                    return make_shared<KSValue>(buffer.str());
                }
                return make_shared<KSValue>();
            }},
        });
    }
}
