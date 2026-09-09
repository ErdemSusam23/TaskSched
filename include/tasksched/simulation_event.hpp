#pragma once

#include "tasksched/job.hpp"

#include <optional>
#include <string>

namespace tasksched {

enum class EventType {
    simulationStarted,
    jobReleased,
    jobDispatched,
    jobPreempted,
    jobCompleted,
    deadlineMissed,
    cpuIdle,
};

struct JobReference {
    JobId id{};
    std::string taskId;
};

// A structured record produced by the simulator, independent of its presentation.
struct SimulationEvent {
    EventType type{};
    int time{};
    std::optional<JobReference> subject;
    std::optional<JobReference> relatedJob;
    std::string detail;
};

} // namespace tasksched
