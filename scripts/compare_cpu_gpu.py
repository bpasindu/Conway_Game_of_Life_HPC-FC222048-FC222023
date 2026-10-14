
import pandas as pd
from pathlib import Path

root = Path(__file__).resolve().parent.parent
results = root / "results"

cpu = pd.read_csv(results / "cpu_benchmark_results.csv")
gpu = pd.read_csv(results / "gpu_benchmark_results.csv")

# Use median times to reduce sensitivity to outliers.
cpu_summary = (
    cpu.groupby("GridSize")
    .agg(
        CPU_MedianMs=("TimeMs", "median"),
        CPU_AverageMs=("TimeMs", "mean"),
        CPU_StdDevMs=("TimeMs", "std"),
    )
    .reset_index()
)

gpu_summary = (
    gpu.groupby("GridSize")
    .agg(
        GPU_MedianKernelMs=("KernelTimeMs", "median"),
        GPU_AverageKernelMs=("KernelTimeMs", "mean"),
        GPU_MedianPipelineMs=("PipelineTimeMs", "median"),
        GPU_AveragePipelineMs=("PipelineTimeMs", "mean"),
    )
    .reset_index()
)

comparison = cpu_summary.merge(gpu_summary, on="GridSize")

comparison["KernelSpeedupRatio"] = (
    comparison["CPU_MedianMs"] /
    comparison["GPU_MedianKernelMs"]
)

comparison["PipelineSpeedupRatio"] = (
    comparison["CPU_MedianMs"] /
    comparison["GPU_MedianPipelineMs"]
)

comparison.to_csv(
    results / "cpu_gpu_comparison.csv", index=False
)

comparison.to_markdown(
    results / "cpu_gpu_comparison.md",
    index=False,
    floatfmt=".3f",
)

print(comparison.round(3).to_string(index=False))
print("\nSaved:")
print(results / "cpu_gpu_comparison.csv")
print(results / "cpu_gpu_comparison.md")
