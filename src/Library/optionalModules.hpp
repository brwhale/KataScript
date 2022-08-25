#pragma once
#include "KataScript.hpp"
#include <string>
#include <thread>

namespace KataScript {
    void KataScriptInterpreter::createOptionalModules() {
        newModule("file", ModulePrivilege::fileSystemRead | ModulePrivilege::fileSystemWrite, {
            { "saveFile", [](const List& args) {
                if (args.size() == 2
                    && args[0]->getType() == Type::String
                    && args[1]->getType() == Type::String
                    ) {
                    auto t = std::ofstream(args[1]->getString(), std::ofstream::out);
                    t << args[0]->getString();
                    t.flush();
                    return make_shared<Value>(true);
                }
                return make_shared<Value>(false);
            }},
            { "readFile", [](const List& args) {
                if (args.size() == 1 && args[0]->getType() == Type::String) {
                    std::stringstream buffer;
                    buffer << std::ifstream(args[0]->getString()).rdbuf();
                    return make_shared<Value>(buffer.str());
                }
                return make_shared<Value>();
            }},
        });

        // currently this kinda works, but the whole scoping system is too state based and it breaks down
        newModule("thread", ModulePrivilege::fileSystemRead | ModulePrivilege::experimental, {
            { "newThread", [this](const List& args) {
                if (args.size() == 1 && args[0]->getType() == Type::Function) {
                    auto func = args[0]->getFunction();
                    auto ptr = new std::thread([this, func]() {
                        callFunction(func, {});
                    });
                    return make_shared<Value>(ptr);
                }
                return make_shared<Value>();
            }},
            { "joinThread", [](const List& args) {
                if (args.size() == 1 && args[0]->getType() == Type::UserPointer) {
                    std::thread* ptr = (std::thread*)args[0]->getPointer();
                    ptr->join();
                    delete ptr;
                }
                return make_shared<Value>();
            }},
        });
    }
}
