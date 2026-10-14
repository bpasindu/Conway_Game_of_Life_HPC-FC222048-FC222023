# HPC Game of Life Project Log

## 1. Assignment requirements

The project requires a CPU implementation of Conway's Game of Life in C++ and a planned CUDA implementation for GPU acceleration. The workflow must include source code, testing, benchmark collection, results export, and project documentation. The repository should preserve the CPU and GPU efforts as separate implementation tracks.

## 2. Project objectives

- Implement a correct CPU reference version of Conway's Game of Life.
- Validate correctness with the standard blinker test.
- Benchmark CPU performance across multiple grid sizes.
- Preserve historical measurements made while using a temporary blinker initialization.
- Prepare documentation and a reproducible benchmark workflow.
- Keep the CUDA implementation pending and clearly distinguished from completed work.

## 3. Development environment

- Operating system: Windows 11 build family, version reported as 10.0.26200.9168.
- C++ compiler: GCC 14.1.0.
- NVIDIA GPU is available locally; exact GPU model is still to be recorded.
- NVIDIA driver version: 528.20.
- `nvidia-smi` reports CUDA version 12.0, but this does not confirm that CUDA Toolkit or `nvcc` is installed.
- `nvcc` is not recognized on the local Windows command line.
- Development platform: VS Code and GitHub.
- Potential CUDA testing environment: Google Colab if needed.

## 4. Hardware specifications

Confirmed facts:

- Windows 11 family build 10.0.26200.9168.
- GCC 14.1.0 available.
- NVIDIA driver version 528.20.
- `nvidia-smi` CUDA version 12.0 observed.
- Local NVIDIA GPU is present.

Unknown or pending:

- Exact GPU model: To be recorded.
- CUDA Toolkit installation status: To be recorded.
- GPU memory and tensor core details: To be recorded.

## 5. Software and installation log

- Verified GCC 14.1.0 is available on the local machine.
- Confirmed the project folder contains the CPU source file and required directories.
- Created the benchmark, test, docs, build, and results directories.
- Prepared a PowerShell benchmarking workflow for compilation, execution, parsing, and statistics.
- CUDA support remains pending because `nvcc` is not currently recognized on the local Windows command line.

## 6. Algorithm and design decisions

- Two-buffer simulation: the current generation and the next generation are stored separately so updates happen simultaneously.
- Toroidal/wraparound boundaries: cells at the edges wrap to the opposite side, matching the assignment requirement.
- Deterministic random initialization: seed 42 is used for all official benchmark runs.
- Initial live density: approximately 30% live cells, matching the project specification.
- Simulation loop timing: the measured time covers only the generation updates, not grid rendering or file writing.
- Random-grid benchmarks are run with display disabled to avoid output overhead.
- A separate blinker test is used for correctness validation and is not treated as the benchmark workload.

## 7. CPU implementation

The CPU version is implemented in [src/game_of_life_cpu.cpp](src/game_of_life_cpu.cpp). It uses a standard grid representation and applies Conway's rules for each cell:

- live cell with 2 or 3 neighbors survives,
- dead cell with exactly 3 neighbors becomes alive,
- all other cells die or remain dead.

The implementation supports:

- grid size and iteration count command-line arguments,
- optional rendering (`show` or `hide`),
- helpful argument validation,
- deterministic random initialization,
- elapsed time measurement with `std::chrono`,
- final live-cell count and cell-updates-per-second reporting.

## 8. CUDA implementation — pending

The GPU implementation remains pending. The project structure is prepared for a CUDA version, and a future GPU benchmark will need to record:

- kernel duration,
- memory transfer time,
- end-to-end runtime,
- GPU speedup relative to CPU time,
- equivalent workload and initialization settings.

Important distinction:

- CPU and GPU results should only be compared when the grid size, iteration count, boundary conditions, and initial grid are equivalent.
- Kernel-only timings and full transfer-inclusive timings must be labeled clearly.

## 9. Testing and correctness

A dedicated blinker correctness test validates standard Game of Life behavior.

The verified test uses the 5 x 5 pattern:

- `(1,2)`, `(2,2)`, `(3,2)`

This should produce:

- horizontal pattern after one generation,
- vertical pattern after two generations,
- live-cell count of 3 after both generations.

The test is stored under [tests/blinker_test.cpp](tests/blinker_test.cpp) and can be compiled and run independently.

## 10. Benchmark methodology

- Grid sizes: 500 x 500, 1000 x 1000, 2000 x 2000, 4000 x 4000.
- Iterations per run: 100.
- Repetitions: 5 per grid size.
- Initial grid: deterministic random with seed 42 and approximately 30% live density.
- Timing: simulation loop only.
- Output suppressed during timed runs.
- Cell update formula: `GridSize x GridSize x Iterations`.
- Throughput formula: `CellUpdates / (TimeMs / 1000)`.
- Summary statistics: valid run count, average, median, minimum, maximum, sample standard deviation, and average throughput.

## 11. Raw and processed results

The following files hold results:

- `results/cpu_benchmark_results.csv` — raw repeated measurements.
- `results/cpu_benchmark_summary.csv` — summary statistics per grid size.
- `results/cpu_benchmark_summary.md` — human-readable summary.
- `results/historical_blinker_runs.csv` — historical blinker-test data, preserved separately.

The historical blinker file is explicitly not part of the official benchmark set because the earlier runs used a temporary non-random blinker initial condition.

## 12. Optimization experiments

Pending:

- compiler optimization review,
- layout and cache analysis,
- potential vectorization opportunities,
- comparison of single-grid versus double-buffer implementation,
- GPU acceleration experiments when CUDA is available.

## 13. Findings and limitations

- CPU implementation is correct and benchmarkable.
- Random benchmark initialization is reproducible and stable with seed 42.
- Time measurement excludes printing and file I/O to match HPC benchmarking practice.
- Historical blinker runs are useful as debugging history but must be excluded from the official CPU benchmark dataset.
- CUDA implementation is not yet validated locally due to the missing `nvcc` toolchain.

## 14. Final report and presentation checklist

- [x] CPU implementation completed.
- [x] Random-init benchmark design confirmed.
- [x] Blinker correctness test created.
- [x] Benchmark script prepared.
- [x] Results and summary outputs defined.
- [x] Historical blinker measurements preserved separately.
- [x] Documentation updated.
- [ ] CUDA implementation and GPU benchmark pending.
- [ ] Final hardware information to be recorded once the exact GPU model is confirmed.
- [ ] Presentation figures to be added once benchmark results are collected.
