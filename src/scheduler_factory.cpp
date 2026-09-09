#include "tasksched/scheduler_factory.hpp"

#include "tasksched/edf_scheduler.hpp"
#include "tasksched/rms_scheduler.hpp"

#include <stdexcept>

namespace tasksched {

std::unique_ptr<IScheduler> makeScheduler(SchedulingPolicy policy) {
    switch (policy) {
    case SchedulingPolicy::rms: return std::make_unique<RmsScheduler>();
    case SchedulingPolicy::edf: return std::make_unique<EdfScheduler>();
    }
    throw std::logic_error("Unknown scheduling policy");
}

} // namespace tasksched
