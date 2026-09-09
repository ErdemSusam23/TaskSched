#pragma once

#include "tasksched/job.hpp"

#include <optional>
#include <vector>

namespace tasksched {

class IScheduler {
public:
    virtual ~IScheduler() = default;
    [[nodiscard]] virtual std::optional<JobId>
    selectNext(const std::vector<Job>& readyJobs) const = 0;
    [[nodiscard]] virtual const char* name() const = 0;
};

} // namespace tasksched
