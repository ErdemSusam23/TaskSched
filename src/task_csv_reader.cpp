#include "tasksched/task_csv_reader.hpp"

#include <charconv>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace tasksched {

namespace {

std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

int parsePositiveInt(const std::string& value, int line, const char* column) {
    int result{};
    const auto [position, error] = std::from_chars(value.data(), value.data() + value.size(), result);
    if (error != std::errc{} || position != value.data() + value.size() || result <= 0) {
        throw std::runtime_error("Invalid " + std::string(column) + " at CSV line " +
                                 std::to_string(line));
    }
    return result;
}

} // namespace

std::vector<Task> TaskCsvReader::load(const std::filesystem::path& inputPath) const {
    std::ifstream input{inputPath};
    if (!input) throw std::runtime_error("Cannot open task CSV: " + inputPath.string());

    std::string line;
    if (!std::getline(input, line) || trim(line) != "id,period,execution,deadline") {
        throw std::runtime_error("CSV header must be: id,period,execution,deadline");
    }

    std::vector<Task> tasks;
    for (int lineNumber = 2; std::getline(input, line); ++lineNumber) {
        if (trim(line).empty() || trim(line).starts_with('#')) continue;

        std::stringstream row{line};
        std::string id;
        std::string period;
        std::string execution;
        std::string deadline;
        std::string extra;
        if (!std::getline(row, id, ',') || !std::getline(row, period, ',') ||
            !std::getline(row, execution, ',') || !std::getline(row, deadline, ',') ||
            std::getline(row, extra, ',')) {
            throw std::runtime_error("Expected four CSV columns at line " + std::to_string(lineNumber));
        }

        Task task{trim(id), 0, parsePositiveInt(trim(period), lineNumber, "period"),
                  parsePositiveInt(trim(execution), lineNumber, "execution"),
                  parsePositiveInt(trim(deadline), lineNumber, "deadline")};
        if (!task.isValid()) throw std::runtime_error("Invalid task at CSV line " + std::to_string(lineNumber));
        tasks.push_back(std::move(task));
    }

    if (tasks.empty()) throw std::runtime_error("Task CSV contains no task definitions");
    return tasks;
}

} // namespace tasksched
