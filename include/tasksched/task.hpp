#pragma once

#include <string>

namespace tasksched {

// A recurring workload definition. A release creates a Job from this template.
struct Task {
    std::string id;
    int basePriority{};       // Reserved for fixed-priority policies other than RMS.
    int periodTicks{};
    int executionTicks{};
    int relativeDeadlineTicks{};

    [[nodiscard]] bool isValid() const {
        return !id.empty() && periodTicks > 0 && executionTicks > 0 &&
               relativeDeadlineTicks > 0;
    }
};

} // namespace tasksched
