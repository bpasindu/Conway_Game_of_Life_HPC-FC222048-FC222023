
from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parent.parent
RESULTS = ROOT / "results"
RESULTS.mkdir(exist_ok=True)

comparison = pd.read_csv(RESULTS / "cpu_gpu_comparison.csv")
sizes = comparison["GridSize"].astype(str)

# Chart 1: Median runtime comparison.
# Use GPU pipeline time for a more practical comparison than kernel-only time.
plt.figure(figsize=(9, 5))
plt.plot(
    sizes, comparison["CPU_MedianMs"],
    marker="o", label="CPU median"
)
plt.plot(
    sizes, comparison["GPU_MedianPipelineMs"],
    marker="o", label="GPU median pipeline"
)
plt.xlabel("Grid size (N x N)")
plt.ylabel("Time (ms)")
plt.title("Conway's Game of Life: CPU vs GPU Runtime")
plt.yscale("log")
plt.grid(True, alpha=0.3)
plt.legend()
plt.tight_layout()
plt.savefig(RESULTS / "cpu_gpu_runtime.png", dpi=200)
plt.close()

# Chart 2: Preliminary ratios, including pipeline timing.
plt.figure(figsize=(9, 5))
plt.plot(
    sizes, comparison["KernelSpeedupRatio"],
    marker="o", label="CPU median / GPU kernel median"
)
plt.plot(
    sizes, comparison["PipelineSpeedupRatio"],
    marker="o", label="CPU median / GPU pipeline median"
)
plt.xlabel("Grid size (N x N)")
plt.ylabel("Time ratio (x)")
plt.title("Preliminary CPU-to-GPU Performance Ratios")
plt.grid(True, alpha=0.3)
plt.legend()
plt.tight_layout()
plt.savefig(RESULTS / "cpu_gpu_speedup.png", dpi=200)
plt.close()

# Report-ready findings.
lines = [
    "# CPU and GPU Benchmark Findings",
    "",
    "Each benchmark used 100 iterations and five repeated runs per grid size.",
    "",
    "The CPU measurements were collected on a local Windows machine. "
    "The GPU measurements were collected in Google Colab on an NVIDIA Tesla T4. "
    "These are cross-machine comparisons, not controlled same-machine speedups.",
    "",
    "The pipeline timing includes device allocation, data transfers, and the "
    "simulation; the kernel timing excludes data transfers.",
    "",
    "| Grid | CPU median (ms) | GPU kernel median (ms) | "
    "GPU pipeline median (ms) | Kernel ratio | Pipeline ratio |",
    "|---:|---:|---:|---:|---:|---:|",
]

for _, row in comparison.iterrows():
    lines.append(
        f"| {int(row['GridSize'])} x {int(row['GridSize'])} "
        f"| {row['CPU_MedianMs']:.3f} "
        f"| {row['GPU_MedianKernelMs']:.3f} "
        f"| {row['GPU_MedianPipelineMs']:.3f} "
        f"| {row['KernelSpeedupRatio']:.2f}x "
        f"| {row['PipelineSpeedupRatio']:.2f}x |"
    )

lines += [
    "",
    "## Limitations",
    "",
    "- CPU and GPU measurements were taken on different machines.",
    "- CPU timing variability was high for the 4000 x 4000 workload.",
    "- Matching final live-cell counts is a consistency check, not proof "
    "that every final cell matches.",
    "- Kernel-only and pipeline timings measure different scopes.",
    "",
]

(RESULTS / "benchmark_findings.md").write_text(
    "\n".join(lines), encoding="utf-8"
)

print("Created:")
print(RESULTS / "cpu_gpu_runtime.png")
print(RESULTS / "cpu_gpu_speedup.png")
print(RESULTS / "benchmark_findings.md")
