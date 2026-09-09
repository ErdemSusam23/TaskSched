#pragma once

#include "tasksched/scheduler.hpp"

namespace tasksched {

class RmsScheduler final : public IScheduler {
public:
    [[nodiscard]] std::optional<JobId>
    selectNext(const std::vector<Job>& readyJobs) const override;
    [[nodiscard]] const char* name() const override { return "RMS"; }
};

} // namespace tasksched
