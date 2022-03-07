#pragma once
#include <exception>
#include <string>
namespace KataScript {
    struct KSException : public std::exception {
        std::string wh;
        KSException(const std::string& w) : wh(w) {};
    };
}