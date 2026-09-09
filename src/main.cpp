#include "tasksched/command_line_parser.hpp"
#include "tasksched/composite_logger.hpp"
#include "tasksched/logger.hpp"
#include "tasksched/scheduler_factory.hpp"
#include "tasksched/simulator.hpp"
#include "tasksched/statistics_reporter.hpp"
#include "tasksched/task_csv_reader.hpp"

#include <iostream>
#include <string_view>
#include <vector>

namespace {

void runScenario(tasksched::SchedulingPolicy policy, const std::vector<tasksched::Task>& tasks,
                 int durationTicks) {
    auto scheduler = tasksched::makeScheduler(policy);
    tasksched::ConsoleLogger consoleLogger;
    tasksched::StatisticsReporter statisticsReporter;
    tasksched::CompositeLogger logger{&consoleLogger, &statisticsReporter};
    tasksched::Simulator simulator{tasks, *scheduler, logger};
    simulator.run(durationTicks);

    const auto& report = statisticsReporter.report();
    std::cout << "Summary for " << scheduler->name() << ": idle=" << report.idleTicks
              << ", preemptions=" << report.totalPreemptions << '\n';
    for (const auto& [taskId, stats] : report.perTask) {
        std::cout << "  " << taskId << ": releases=" << stats.releases
                  << ", dispatchedTicks=" << stats.dispatchedTicks
                  << ", completed=" << stats.completions
                  << ", misses=" << stats.deadlineMisses
                  << ", preempted=" << stats.preemptions << '\n';
    }
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        std::vector<std::string_view> arguments;
        for (int index = 1; index < argc; ++index) arguments.emplace_back(argv[index]);

        const tasksched::CommandLineParser commandLineParser;
        const auto options = commandLineParser.parse(arguments);
        if (options.showHelp) {
            std::cout << tasksched::CommandLineParser::usage() << '\n';
            return 0;
        }

        const tasksched::TaskCsvReader reader;
        const auto tasks = reader.load(options.inputPath);

        std::cout << "Loaded " << tasks.size() << " tasks from " << options.inputPath.string() << "\n\n";
        if (options.policy == tasksched::PolicySelection::rms ||
            options.policy == tasksched::PolicySelection::both) {
            runScenario(tasksched::SchedulingPolicy::rms, tasks, options.durationTicks);
        }
        if (options.policy == tasksched::PolicySelection::edf ||
            options.policy == tasksched::PolicySelection::both) {
            runScenario(tasksched::SchedulingPolicy::edf, tasks, options.durationTicks);
        }
    } catch (const std::exception& error) {
        std::cerr << "TaskSched error: " << error.what() << '\n';
        return 1;
    }
}
