# CPU Benchmark Summary

- Iterations per run: 100
- Repetitions per grid size: 5
- Initial grid: deterministic random grid, seed 42, 30% live probability
- Boundary condition: toroidal (wraparound)
- Timing: simulation loop only; grid display disabled
- Standard deviation: sample standard deviation (N - 1)

| Grid size | Valid runs | Average (ms) | Median (ms) | Min (ms) | Max (ms) | Sample SD (ms) | Average throughput (cells/s) |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 500 x 500 | 5 | 544.803 | 546.472 | 537.301 | 551.098 | 6.343 | 45888146.725 |
| 1000 x 1000 | 5 | 2161.596 | 2161.005 | 2138.087 | 2194.253 | 23.724 | 46262118.015 |
| 2000 x 2000 | 5 | 8589.86 | 8584.414 | 8544.751 | 8652.717 | 39.253 | 46566530.93 |
| 4000 x 4000 | 5 | 45671.659 | 47046.664 | 34271.13 | 57631.598 | 8744.393 | 35032666.845 |

Raw measurements are stored in cpu_benchmark_results.csv.
These results must not be mixed with historical blinker-test timings.
