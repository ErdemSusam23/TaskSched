#pragma once

#include <cstdint>
#include <string>

namespace tasksched {

using JobId = std::uint64_t;

// One concrete execution request released by a periodic Task.
struct Job {
    JobId id{};
    std::string taskId;
    int periodTicks{};
    int releaseTime{};
    int absoluteDeadline{};
    int remainingTicks{};
    bool deadlineMissReported{false};

    [[nodiscard]] bool isComplete() const { return remainingTicks == 0; }
};

} // namespace tasksched
