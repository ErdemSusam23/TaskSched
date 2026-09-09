#include "tasksched/simulator.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace tasksched {

namespace {

JobReference referenceFor(const Job& job) {
    return {job.id, job.taskId};
}

} // namespace

Simulator::Simulator(std::vector<Task> tasks, const IScheduler& scheduler, ILogger& logger)
    : tasks_(std::move(tasks)), scheduler_(scheduler), logger_(logger) {
    if (std::any_of(tasks_.begin(), tasks_.end(), [](const Task& task) { return !task.isValid(); })) {
        throw std::invalid_argument("Every task must have valid timing properties");
    }
}

void Simulator::run(int durationTicks) {
    if (durationTicks < 0) throw std::invalid_argument("Duration cannot be negative");

    logger_.log({.type = EventType::simulationStarted,
                 .time = 0,
                 .subject = std::nullopt,
                 .relatedJob = std::nullopt,
                 .detail = scheduler_.name()});
    for (int now = 0; now < durationTicks; ++now) {
        releaseDueJobs(now);
        reportDeadlineMisses(now);

        std::vector<Job> ready;
        for (const auto& job : jobs_) {
            if (!job.isComplete() && job.releaseTime <= now) ready.push_back(job);
        }

        const auto selectedId = scheduler_.selectNext(ready);
        if (!selectedId) {
            logger_.log({.type = EventType::cpuIdle,
                         .time = now,
                         .subject = std::nullopt,
                         .relatedJob = std::nullopt,
                         .detail = {}});
            continue;
        }

        const auto selected = std::find_if(jobs_.begin(), jobs_.end(),
            [id = *selectedId](const Job& job) { return job.id == id; });

        if (previouslyRunningJobId_ && *previouslyRunningJobId_ != selected->id) {
            const auto previous = std::find_if(jobs_.begin(), jobs_.end(),
                [id = *previouslyRunningJobId_](const Job& job) { return job.id == id; });
            if (previous != jobs_.end() && !previous->isComplete()) {
                logger_.log({.type = EventType::jobPreempted,
                             .time = now,
                             .subject = referenceFor(*previous),
                             .relatedJob = referenceFor(*selected),
                             .detail = {}});
            }
        }

        --selected->remainingTicks;
        logger_.log({.type = EventType::jobDispatched,
                     .time = now,
                     .subject = referenceFor(*selected),
                     .relatedJob = std::nullopt,
                     .detail = {}});
        previouslyRunningJobId_ = selected->id;
        if (selected->isComplete()) {
            logger_.log({.type = EventType::jobCompleted, .time = now + 1,
                         .subject = referenceFor(*selected),
                         .relatedJob = std::nullopt,
                         .detail = {}});
            previouslyRunningJobId_.reset();
        }
    }
}

void Simulator::releaseDueJobs(int now) {
    for (const auto& task : tasks_) {
        if (now % task.periodTicks == 0) {
            jobs_.push_back({nextJobId_++, task.id, task.periodTicks, now,
                             now + task.relativeDeadlineTicks, task.executionTicks});
            logger_.log({.type = EventType::jobReleased, .time = now,
                         .subject = referenceFor(jobs_.back()),
                         .relatedJob = std::nullopt,
                         .detail = {}});
        }
    }
}

void Simulator::reportDeadlineMisses(int now) {
    for (auto& job : jobs_) {
        if (!job.isComplete() && !job.deadlineMissReported && now >= job.absoluteDeadline) {
            job.deadlineMissReported = true;
            logger_.log({.type = EventType::deadlineMissed, .time = now,
                         .subject = referenceFor(job),
                         .relatedJob = std::nullopt,
                         .detail = {}});
        }
    }
}

} // namespace tasksched
