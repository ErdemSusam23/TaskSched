#include "tasksched/logger.hpp"
#include "tasksched/scheduler_factory.hpp"
#include "tasksched/simulator.hpp"

#include <memory>
#include <string>
#include <vector>

namespace {

void runScenario(tasksched::SchedulingPolicy policy, const std::vector<tasksched::Task>& tasks) {
    auto scheduler = tasksched::makeScheduler(policy);
    tasksched::ConsoleLogger logger;
    tasksched::Simulator simulator{tasks, *scheduler, logger};
    simulator.run(12);
}

} // namespace

int main() {
    // Constrained deadlines intentionally make RMS and EDF choose differently at t=0.
    const std::vector tasks{
        tasksched::Task{"control", 0, 5, 2, 5},
        tasksched::Task{"alarm", 0, 8, 1, 3},
        tasksched::Task{"telemetry", 0, 10, 4, 10},
    };

    runScenario(tasksched::SchedulingPolicy::rms, tasks);
    runScenario(tasksched::SchedulingPolicy::edf, tasks);
}
