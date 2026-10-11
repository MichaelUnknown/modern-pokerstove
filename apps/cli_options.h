#pragma once

#include <cxxopts.hpp>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace pokerstove::cli
{
inline cxxopts::ParseResult parse(cxxopts::Options& options, int argc, char** argv,
                                  std::initializer_list<const char*> unique_options)
{
    // cxxopts reserves alphanumeric short names; preserve the legacy -? alias.
    std::vector<const char*> arguments(argv, argv + argc);
    bool positional_only = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::string_view(arguments[i]) == "--")
            positional_only = true;
        else if (!positional_only && std::string_view(arguments[i]) == "-?")
            arguments[i] = "--help";
    }
    auto result = options.parse(argc, arguments.data());
    if (!result.unmatched().empty())
        throw std::invalid_argument("Unexpected argument: " + result.unmatched().front());
    for (const auto name : unique_options)
        if (result.count(name) > 1)
            throw std::invalid_argument(std::string("Option specified more than once: ") + name);
    return result;
}
}  // namespace pokerstove::cli
