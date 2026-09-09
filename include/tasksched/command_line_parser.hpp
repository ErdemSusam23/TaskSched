#pragma once

#include <filesystem>
#include <span>
#include <string_view>

namespace tasksched {

enum class PolicySelection { rms, edf, both };

struct ApplicationOptions {
    std::filesystem::path inputPath{"examples/default_tasks.csv"};
    PolicySelection policy{PolicySelection::both};
    int durationTicks{12};
    bool showHelp{false};
};

class CommandLineParser {
public:
    [[nodiscard]] ApplicationOptions parse(std::span<const std::string_view> arguments) const;
    [[nodiscard]] static std::string_view usage();
};

} // namespace tasksched
