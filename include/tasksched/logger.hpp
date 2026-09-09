#pragma once

#include "tasksched/simulation_event.hpp"

namespace tasksched {

class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const SimulationEvent& event) = 0;
};

class ConsoleLogger final : public ILogger {
public:
    void log(const SimulationEvent& event) override;
};

} // namespace tasksched
