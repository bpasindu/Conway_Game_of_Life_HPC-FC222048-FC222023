# CPU and GPU Benchmark Findings

Each benchmark used 100 iterations and five repeated runs per grid size.

The CPU measurements were collected on a local Windows machine. The GPU measurements were collected in Google Colab on an NVIDIA Tesla T4. These are cross-machine comparisons, not controlled same-machine speedups.

The pipeline timing includes device allocation, data transfers, and the simulation; the kernel timing excludes data transfers.

| Grid | CPU median (ms) | GPU kernel median (ms) | GPU pipeline median (ms) | Kernel ratio | Pipeline ratio |
|---:|---:|---:|---:|---:|---:|
| 500 x 500 | 546.472 | 2.084 | 2.381 | 262.22x | 229.49x |
| 1000 x 1000 | 2161.005 | 4.322 | 5.137 | 500.05x | 420.66x |
| 2000 x 2000 | 8584.414 | 17.406 | 20.246 | 493.19x | 424.00x |
| 4000 x 4000 | 47046.664 | 55.522 | 65.563 | 847.35x | 717.58x |

## Limitations

- CPU and GPU measurements were taken on different machines.
- CPU timing variability was high for the 4000 x 4000 workload.
- Matching final live-cell counts is a consistency check, not proof that every final cell matches.
- Kernel-only and pipeline timings measure different scopes.
