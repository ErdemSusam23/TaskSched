#include "tasksched/edf_scheduler.hpp"
#include "tasksched/rms_scheduler.hpp"
#include "tasksched/simulator.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

tasksched::Job job(tasksched::JobId id, int period, int deadline) {
    return {id, "task", period, 0, deadline, 1};
}

class RecordingLogger final : public tasksched::ILogger {
public:
    void log(const tasksched::SimulationEvent& event) override { events.push_back(event); }

    std::vector<tasksched::SimulationEvent> events;
};

} // namespace

int main() {
    const std::vector jobs{job(1, 10, 4), job(2, 4, 9), job(3, 5, 3)};

    tasksched::RmsScheduler rms;
    require(rms.selectNext(jobs) == 2, "RMS must select the shortest-period job");

    tasksched::EdfScheduler edf;
    require(edf.selectNext(jobs) == 3, "EDF must select the earliest-deadline job");

    // Execution demand exceeds the period: the first job remains unfinished at t=2.
    RecordingLogger logger;
    tasksched::Simulator simulator{{{"overloaded", 0, 2, 3, 2}}, rms, logger};
    simulator.run(3);
    const bool deadlineMissed = std::any_of(logger.events.begin(), logger.events.end(),
        [](const tasksched::SimulationEvent& event) {
            return event.type == tasksched::EventType::deadlineMissed;
        });
    require(deadlineMissed,
            "Simulator must report a job that reaches its deadline unfinished");

    RecordingLogger preemptionLogger;
    tasksched::Simulator preemptionSimulator{
        {{"background", 0, 10, 4, 10}, {"urgent", 0, 2, 1, 2}}, rms, preemptionLogger};
    preemptionSimulator.run(3);
    const auto preemption = std::find_if(preemptionLogger.events.begin(), preemptionLogger.events.end(),
        [](const tasksched::SimulationEvent& event) {
            return event.type == tasksched::EventType::jobPreempted;
        });
    require(preemption != preemptionLogger.events.end() && preemption->time == 2 &&
                preemption->subject->taskId == "background" &&
                preemption->relatedJob->taskId == "urgent",
            "RMS preemption must identify the interrupted and newly selected jobs");
}
