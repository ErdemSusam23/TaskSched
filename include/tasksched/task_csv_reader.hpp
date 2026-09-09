#pragma once

#include "tasksched/task.hpp"

#include <filesystem>
#include <vector>

namespace tasksched {

class TaskCsvReader {
public:
    [[nodiscard]] std::vector<Task> load(const std::filesystem::path& inputPath) const;
};

} // namespace tasksched
