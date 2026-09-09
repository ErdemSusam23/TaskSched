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
