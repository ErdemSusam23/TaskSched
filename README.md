# TaskSched

A small, deterministic C++20 simulator for preemptive periodic real-time scheduling. It compares Rate Monotonic Scheduling (RMS) with Earliest Deadline First (EDF), and reports deadline misses.

The architecture, assumptions, UML diagram and design-pattern rationale are in [docs/architecture.md](docs/architecture.md).

## Build and run

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
.\build\tasksched_demo.exe
```

## Example runs

```powershell
.\tasksched_demo.exe --input examples\default_tasks.csv --policy both --duration 12
.\tasksched_demo.exe --input examples\edf_advantage.csv --policy both --duration 15
.\tasksched_demo.exe --input examples\overloaded.csv --policy rms --duration 8
```
