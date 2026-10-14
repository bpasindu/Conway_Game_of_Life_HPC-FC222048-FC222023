# Conway's Game of Life HPC

This project explores Conway's Game of Life on the CPU and prepares for a CUDA implementation. The current repository focuses on the CPU reference implementation and workflow for benchmarking, testing, and documentation.

## Folder structure

```text
Conway_Game_of_Life_HPC/
├── src/
│   ├── game_of_life_cpu.cpp
│   └── game_of_life_cpu.hpp
├── tests/
│   └── blinker_test.cpp
├── scripts/
│   └── run_cpu_benchmarks.ps1
├── data/
├── results/
├── docs/
│   └── HPC_Game_of_Life_Project_Log.md
├── build/
├── README.md
├── .gitignore
└── historical_blinker_runs.csv
```

## Requirements

- Windows 11 (build family 10.0.26200.9168)
- GCC 14.1.0
- C++17 compatible compiler
- PowerShell 5+ or PowerShell 7+
- Optional: NVIDIA GPU and CUDA environment for a future GPU implementation

## CPU program

The CPU implementation is in [src/game_of_life_cpu.cpp](src/game_of_life_cpu.cpp). It uses a two-buffer simulation, toroidal wraparound boundaries, deterministic random initialization with seed 42, and a default live-cell probability of approximately 30%.

### Compile the CPU version

```powershell
g++ src/game_of_life_cpu.cpp -std=c++17 -O2 -Wall -Wextra -o build/game_of_life_cpu.exe
```

### Run the CPU version

```powershell
./build/game_of_life_cpu.exe 500 100 hide
```

Arguments:

- grid size: integer N for an N x N grid
- iteration count: integer number of generations
- display mode: `show` or `hide`

Example:

```powershell
./build/game_of_life_cpu.exe 20 10 show
```

## Automated benchmark script

The benchmark script is in [scripts/run_cpu_benchmarks.ps1](scripts/run_cpu_benchmarks.ps1). It compiles the CPU program, runs five repetitions for each benchmark size, parses timing data, writes raw results, and calculates summary statistics.

Run it with:

```powershell
pwsh ./scripts/run_cpu_benchmarks.ps1
```

## Results

The benchmark output is stored in the results folder:

- `results/cpu_benchmark_results.csv`
- `results/cpu_benchmark_summary.csv`
- `results/cpu_benchmark_summary.md`
- `results/historical_blinker_runs.csv`

The historical blinker file preserves earlier debugging measurements that used the temporary blinker initialization; they are not part of the official random-grid benchmark dataset.

## Correctness test

A separate blinker test validates the standard Game of Life transition rules without relying on display output.

Compile and execute:

```powershell
g++ tests/blinker_test.cpp src/game_of_life_cpu.cpp -std=c++17 -O2 -Wall -Wextra -o build/blinker_test.exe
./build/blinker_test.exe
```

The test checks that:

- a 5 x 5 blinker pattern is vertical at time 0,
- after one generation it becomes horizontal,
- after two generations it returns to vertical,
- the live-cell count remains 3 across both generations.

## CUDA status

The CUDA implementation is pending. This repository is prepared for future GPU work and may be extended using Google Colab if CUDA Toolkit is not available locally.

## Notes

- Benchmark timing excludes grid printing and file writing.
- The random benchmark uses seed 42 and approximately 30% initial live density.
- Boundary conditions are toroidal/wraparound.
- Timing uses `std::chrono` and reports elapsed milliseconds.
