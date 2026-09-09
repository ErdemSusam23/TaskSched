#include "tasksched/command_line_parser.hpp"

#include <charconv>
#include <stdexcept>
#include <string>

namespace tasksched {

namespace {

int parsePositiveInt(std::string_view value, const char* optionName) {
    int result{};
    const auto [position, error] = std::from_chars(value.data(), value.data() + value.size(), result);
    if (error != std::errc{} || position != value.data() + value.size() || result <= 0) {
        throw std::runtime_error(std::string(optionName) + " must be a positive integer");
    }
    return result;
}

PolicySelection parsePolicy(std::string_view value) {
    if (value == "rms") return PolicySelection::rms;
    if (value == "edf") return PolicySelection::edf;
    if (value == "both") return PolicySelection::both;
    throw std::runtime_error("--policy must be rms, edf, or both");
}

} // namespace

ApplicationOptions CommandLineParser::parse(std::span<const std::string_view> arguments) const {
    ApplicationOptions options;
    for (std::size_t index = 0; index < arguments.size(); ++index) {
        const auto option = arguments[index];
        if (option == "--help") {
            options.showHelp = true;
            continue;
        }
        if (index + 1 == arguments.size()) {
            throw std::runtime_error("Missing value for " + std::string(option));
        }

        const auto value = arguments[++index];
        if (option == "--input") {
            options.inputPath = value;
        } else if (option == "--policy") {
            options.policy = parsePolicy(value);
        } else if (option == "--duration") {
            options.durationTicks = parsePositiveInt(value, "--duration");
        } else {
            throw std::runtime_error("Unknown option: " + std::string(option));
        }
    }
    return options;
}

std::string_view CommandLineParser::usage() {
    return "Usage: tasksched_demo [--input <file>] [--policy rms|edf|both] [--duration <ticks>]";
}

} // namespace tasksched
