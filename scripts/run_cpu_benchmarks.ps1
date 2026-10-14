
$ErrorActionPreference = "Stop"

# Run from the project root, regardless of the current directory.
$ProjectRoot = Split-Path -Parent $PSScriptRoot
Set-Location $ProjectRoot

$Source = Join-Path $ProjectRoot "src/game_of_life_cpu.cpp"
$BuildDir = Join-Path $ProjectRoot "build"
$Exe = Join-Path $BuildDir "game_of_life_cpu.exe"
$ResultsDir = Join-Path $ProjectRoot "results"

$Iterations = 100
$RunsPerSize = 5
$GridSizes = @(500, 1000, 2000, 4000)

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
New-Item -ItemType Directory -Force -Path $ResultsDir | Out-Null

if (-not (Test-Path $Source)) {
    throw "Source file not found: $Source"
}

Write-Host "Compiling CPU implementation..." -ForegroundColor Cyan

& g++ -std=c++17 -O2 -Wall -Wextra $Source -o $Exe

if ($LASTEXITCODE -ne 0) {
    throw "Compilation failed. Benchmarks were not started."
}

# Verify the correctness test before collecting performance results.
Write-Host "`nRunning blinker correctness test..." -ForegroundColor Cyan
$TestOutput = & $Exe --blinker-test 2>&1

if ($LASTEXITCODE -ne 0 -or
    (($TestOutput -join "`n") -notmatch "Blinker correctness test passed")) {
    throw "Blinker correctness test failed. Benchmarks were not started."
}

Write-Host "Blinker correctness test passed." -ForegroundColor Green

$Culture = [Globalization.CultureInfo]::InvariantCulture
$RawResults = [System.Collections.Generic.List[object]]::new()

foreach ($Size in $GridSizes) {
    Write-Host "`nBenchmarking ${Size} x ${Size}..." -ForegroundColor Cyan

    for ($Run = 1; $Run -le $RunsPerSize; $Run++) {
        Write-Host "  Run $Run of $RunsPerSize"

        $Output = & $Exe $Size $Iterations hide 2>&1
        $ExitCode = $LASTEXITCODE
        $Text = $Output -join "`n"

        if ($ExitCode -ne 0) {
            throw "Benchmark failed for grid $Size, run $Run. Output: $Text"
        }

        if ($Text -notmatch 'Simulation time:\s*([0-9]+(?:\.[0-9]+)?)\s*ms') {
            throw "Could not parse simulation time for grid $Size, run $Run."
        }

        $TimeMs = [double]::Parse($Matches[1], $Culture)

        if ($Text -notmatch 'Final live cells:\s*(\d+)') {
            throw "Could not parse final live-cell count for grid $Size, run $Run."
        }

        $FinalLiveCells = [long]::Parse($Matches[1], $Culture)
        $CellUpdates = [long]$Size * $Size * $Iterations

        if ($TimeMs -le 0) {
            throw "Invalid timing for grid $Size, run $Run."
        }

        $Throughput = $CellUpdates / ($TimeMs / 1000.0)

        $RawResults.Add([pscustomobject]@{
            GridSize              = $Size
            Iterations            = $Iterations
            Run                   = $Run
            TimeMs                = $TimeMs
            FinalLiveCells        = $FinalLiveCells
            CellUpdates           = $CellUpdates
            CellUpdatesPerSecond  = $Throughput
            Seed                  = 42
            Initialization       = "Random, 30% live probability"
            Notes                 = "Toroidal boundary; grid display disabled"
        })
    }
}

# Save raw measurements. Use full numeric precision in calculations.
$RawPath = Join-Path $ResultsDir "cpu_benchmark_results.csv"
$RawResults | Export-Csv -Path $RawPath -NoTypeInformation -Encoding UTF8

$Summary = foreach ($Size in $GridSizes) {
    $Group = @($RawResults | Where-Object { $_.GridSize -eq $Size })
    $Times = @($Group | ForEach-Object { [double]$_.TimeMs } | Sort-Object)

    $Average = ($Times | Measure-Object -Average).Average
    $Minimum = ($Times | Measure-Object -Minimum).Minimum
    $Maximum = ($Times | Measure-Object -Maximum).Maximum

    if ($Times.Count % 2 -eq 1) {
        $Median = $Times[[int][math]::Floor($Times.Count / 2)]
    } else {
        $Median = (
            $Times[$Times.Count / 2 - 1] +
            $Times[$Times.Count / 2]
        ) / 2.0
    }

    $SumSquaredDifferences = 0.0
    foreach ($Time in $Times) {
        $SumSquaredDifferences += [math]::Pow($Time - $Average, 2)
    }

    # Sample standard deviation: divide by N - 1.
    $StdDev = if ($Times.Count -gt 1) {
        [math]::Sqrt($SumSquaredDifferences / ($Times.Count - 1))
    } else {
        0.0
    }

    $UpdatesPerRun = [long]$Size * $Size * $Iterations
    $AverageThroughput = $UpdatesPerRun / ($Average / 1000.0)

    [pscustomobject]@{
        GridSize                     = $Size
        Iterations                   = $Iterations
        ValidRuns                    = $Times.Count
        AverageMs                    = [math]::Round($Average, 3)
        MedianMs                     = [math]::Round($Median, 3)
        MinimumMs                    = [math]::Round($Minimum, 3)
        MaximumMs                    = [math]::Round($Maximum, 3)
        SampleStdDevMs               = [math]::Round($StdDev, 3)
        AverageThroughputCellsPerSec = [math]::Round($AverageThroughput, 3)
    }
}

$SummaryPath = Join-Path $ResultsDir "cpu_benchmark_summary.csv"
$Summary | Export-Csv -Path $SummaryPath -NoTypeInformation -Encoding UTF8

# Create a readable Markdown report for the repository.
$MarkdownPath = Join-Path $ResultsDir "cpu_benchmark_summary.md"

$Lines = [System.Collections.Generic.List[string]]::new()
$Lines.Add("# CPU Benchmark Summary")
$Lines.Add("")
$Lines.Add("- Iterations per run: $Iterations")
$Lines.Add("- Repetitions per grid size: $RunsPerSize")
$Lines.Add("- Initial grid: deterministic random grid, seed 42, 30% live probability")
$Lines.Add("- Boundary condition: toroidal (wraparound)")
$Lines.Add("- Timing: simulation loop only; grid display disabled")
$Lines.Add("- Standard deviation: sample standard deviation (N - 1)")
$Lines.Add("")
$Lines.Add("| Grid size | Valid runs | Average (ms) | Median (ms) | Min (ms) | Max (ms) | Sample SD (ms) | Average throughput (cells/s) |")
$Lines.Add("|---:|---:|---:|---:|---:|---:|---:|---:|")

foreach ($Row in $Summary) {
    $Lines.Add(
        "| $($Row.GridSize) x $($Row.GridSize) | $($Row.ValidRuns) | " +
        "$($Row.AverageMs) | $($Row.MedianMs) | $($Row.MinimumMs) | " +
        "$($Row.MaximumMs) | $($Row.SampleStdDevMs) | " +
        "$($Row.AverageThroughputCellsPerSec) |"
    )
}

$Lines.Add("")
$Lines.Add("Raw measurements are stored in `cpu_benchmark_results.csv`.")
$Lines.Add("These results must not be mixed with historical blinker-test timings.")

[System.IO.File]::WriteAllLines($MarkdownPath, $Lines)

Write-Host "`nCPU benchmarks completed." -ForegroundColor Green
Write-Host "Raw results:     $RawPath"
Write-Host "Summary CSV:     $SummaryPath"
Write-Host "Summary Markdown: $MarkdownPath"

Write-Host "`nSummary:" -ForegroundColor Cyan
$Summary | Format-Table -AutoSize
