#include "tasksched/edf_scheduler.hpp"

#include <algorithm>

namespace tasksched {

std::optional<JobId> EdfScheduler::selectNext(const std::vector<Job>& readyJobs) const {
    if (readyJobs.empty()) return std::nullopt;

    const auto selected = std::min_element(readyJobs.begin(), readyJobs.end(),
        [](const Job& left, const Job& right) {
            if (left.absoluteDeadline != right.absoluteDeadline) {
                return left.absoluteDeadline < right.absoluteDeadline;
            }
            if (left.releaseTime != right.releaseTime) return left.releaseTime < right.releaseTime;
            return left.id < right.id;
        });
    return selected->id;
}

} // namespace tasksched
