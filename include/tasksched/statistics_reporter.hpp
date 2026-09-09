#pragma once

#include "tasksched/logger.hpp"

#include <map>
#include <string>

namespace tasksched {

struct TaskStatistics {
    int releases{};
    int dispatchedTicks{};
    int completions{};
    int deadlineMisses{};
    int preemptions{};
};

struct SimulationReport {
    int idleTicks{};
    int totalPreemptions{};
    std::map<std::string, TaskStatistics> perTask;
};

// Converts simulator events into algorithm-independent summary metrics.
class StatisticsReporter final : public ILogger {
public:
    void log(const SimulationEvent& event) override;
    [[nodiscard]] const SimulationReport& report() const { return report_; }

private:
    SimulationReport report_;
};

} // namespace tasksched
