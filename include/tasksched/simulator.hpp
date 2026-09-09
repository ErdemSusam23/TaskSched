#pragma once

#include "tasksched/logger.hpp"
#include "tasksched/scheduler.hpp"
#include "tasksched/task.hpp"

#include <optional>
#include <vector>

namespace tasksched {

class Simulator {
public:
    Simulator(std::vector<Task> tasks, const IScheduler& scheduler, ILogger& logger);

    void run(int durationTicks);

private:
    void releaseDueJobs(int now);
    void reportDeadlineMisses(int now);

    std::vector<Task> tasks_;
    std::vector<Job> jobs_;
    const IScheduler& scheduler_;
    ILogger& logger_;
    JobId nextJobId_{1};
    std::optional<JobId> previouslyRunningJobId_;
};

} // namespace tasksched
