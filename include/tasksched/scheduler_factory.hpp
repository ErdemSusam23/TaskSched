#pragma once

#include "tasksched/scheduler.hpp"

#include <memory>

namespace tasksched {

enum class SchedulingPolicy { rms, edf };

[[nodiscard]] std::unique_ptr<IScheduler> makeScheduler(SchedulingPolicy policy);

} // namespace tasksched
