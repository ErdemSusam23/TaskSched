#include "tasksched/logger.hpp"

#include <iostream>

namespace tasksched {

namespace {

std::string label(const JobReference& job) {
    return job.taskId + " (job " + std::to_string(job.id) + ")";
}

} // namespace

void ConsoleLogger::log(const SimulationEvent& event) {
    std::cout << "[t=" << event.time << "] ";
    switch (event.type) {
    case EventType::simulationStarted:
        std::cout << "Simulation started with " << event.detail;
        break;
    case EventType::jobReleased:
        std::cout << "Released " << label(*event.subject);
        break;
    case EventType::jobDispatched:
        std::cout << "Running " << label(*event.subject);
        break;
    case EventType::jobPreempted:
        std::cout << "Preempted " << label(*event.subject) << " by " << label(*event.relatedJob);
        break;
    case EventType::jobCompleted:
        std::cout << "Completed " << label(*event.subject);
        break;
    case EventType::deadlineMissed:
        std::cout << "DEADLINE MISS: " << label(*event.subject);
        break;
    case EventType::cpuIdle:
        std::cout << "CPU idle";
        break;
    }
    std::cout << '\n';
}

} // namespace tasksched
