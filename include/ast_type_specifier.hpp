#pragma once

#include <string_view>
#include <stdexcept>

namespace ast {

enum class TypeSpecifier
{
    INT,
    UNSIGNED,
    FLOAT,
    DOUBLE,
    CHAR,
    ENUM,
    VOID
};

template<typename LogStream>
LogStream& operator<<(LogStream& ls, const TypeSpecifier& type)
{
    const auto TypeToString = [&type] {
        switch (type)
        {
        case TypeSpecifier::INT:
            return "int";
        case TypeSpecifier::UNSIGNED:
            return "unsigned";
        case TypeSpecifier::FLOAT:
            return "float";
        case TypeSpecifier::ENUM:
            return "enum";
        case TypeSpecifier::DOUBLE:
            return "double";
        case TypeSpecifier::CHAR:
            return "char";
        case TypeSpecifier::VOID:
            return "void";
        }
        throw std::runtime_error("Unexpected type specifier");
    };
    return ls << TypeToString();
}

}
