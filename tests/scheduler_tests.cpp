#include "tasksched/command_line_parser.hpp"
#include "tasksched/edf_scheduler.hpp"
#include "tasksched/rms_scheduler.hpp"
#include "tasksched/simulator.hpp"
#include "tasksched/statistics_reporter.hpp"
#include "tasksched/task_csv_reader.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
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

    tasksched::StatisticsReporter reporter;
    for (const auto& event : preemptionLogger.events) reporter.log(event);
    const auto& report = reporter.report();
    require(report.totalPreemptions == 1 && report.perTask.at("background").preemptions == 1,
            "Statistics reporter must count preemptions from structured events");

    const auto csvPath = std::filesystem::temp_directory_path() / "tasksched_valid_tasks.csv";
    {
        std::ofstream csv{csvPath};
        csv << "id,period,execution,deadline\nreader-test,7,2,5\n";
    }
    const tasksched::TaskCsvReader reader;
    const auto loadedTasks = reader.load(csvPath);
    std::filesystem::remove(csvPath);
    require(loadedTasks.size() == 1 && loadedTasks.front().id == "reader-test" &&
                loadedTasks.front().periodTicks == 7 && loadedTasks.front().executionTicks == 2 &&
                loadedTasks.front().relativeDeadlineTicks == 5,
            "CSV reader must map each task column to the domain model");

    const tasksched::CommandLineParser commandLineParser;
    const std::vector<std::string_view> arguments{
        "--input", "custom.csv", "--policy", "edf", "--duration", "50"};
    const auto options = commandLineParser.parse(arguments);
    require(options.inputPath == "custom.csv" && options.policy == tasksched::PolicySelection::edf &&
                options.durationTicks == 50,
            "Command-line parser must map options to application configuration");

    const std::vector deadlineSensitiveTasks{
        tasksched::Task{"sensor", 0, 3, 1, 3},
        tasksched::Task{"actuator", 0, 5, 2, 2},
    };
    tasksched::StatisticsReporter rmsReporter;
    tasksched::Simulator rmsSimulator{deadlineSensitiveTasks, rms, rmsReporter};
    rmsSimulator.run(15);

    tasksched::StatisticsReporter edfReporter;
    tasksched::Simulator edfSimulator{deadlineSensitiveTasks, edf, edfReporter};
    edfSimulator.run(15);
    require(rmsReporter.report().perTask.at("actuator").deadlineMisses > 0 &&
                edfReporter.report().perTask.at("actuator").deadlineMisses == 0,
            "EDF must meet the actuator's constrained deadline where RMS misses it");
}
#include "tasksched/statistics_reporter.hpp"
