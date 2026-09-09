#include "tasksched/composite_logger.hpp"

#include <stdexcept>

namespace tasksched {

CompositeLogger::CompositeLogger(std::initializer_list<ILogger*> loggers) : loggers_(loggers) {
    for (const auto* logger : loggers_) {
        if (logger == nullptr) throw std::invalid_argument("A composite logger cannot contain null");
    }
}

void CompositeLogger::log(const SimulationEvent& event) {
    for (auto* logger : loggers_) logger->log(event);
}

} // namespace tasksched
