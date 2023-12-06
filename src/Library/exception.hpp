#pragma once
#include <exception>
#include <string>

namespace KataScript {
    struct Exception : public std::exception {
        std::string wh;
        Exception(const std::string& w) {
            wh = w;
        };
    };
}