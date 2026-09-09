#pragma once

#include "tasksched/logger.hpp"

#include <initializer_list>
#include <vector>

namespace tasksched {

// Forwards each event to all non-owning logger sinks.
class CompositeLogger final : public ILogger {
public:
    CompositeLogger(std::initializer_list<ILogger*> loggers);

    void log(const SimulationEvent& event) override;

private:
    std::vector<ILogger*> loggers_;
};

} // namespace tasksched
