#include "tasksched/statistics_reporter.hpp"

namespace tasksched {

void StatisticsReporter::log(const SimulationEvent& event) {
    switch (event.type) {
    case EventType::jobReleased:
        ++report_.perTask[event.subject->taskId].releases;
        break;
    case EventType::jobDispatched:
        ++report_.perTask[event.subject->taskId].dispatchedTicks;
        break;
    case EventType::jobPreempted:
        ++report_.totalPreemptions;
        ++report_.perTask[event.subject->taskId].preemptions;
        break;
    case EventType::jobCompleted:
        ++report_.perTask[event.subject->taskId].completions;
        break;
    case EventType::deadlineMissed:
        ++report_.perTask[event.subject->taskId].deadlineMisses;
        break;
    case EventType::cpuIdle:
        ++report_.idleTicks;
        break;
    case EventType::simulationStarted:
        break;
    }
}

} // namespace tasksched
