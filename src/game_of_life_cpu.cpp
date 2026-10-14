
#include "game_of_life_cpu.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace chrono;

int countNeighbours(const Grid& grid, int row, int col, int n) {
    int count = 0;

    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) {
                continue;
            }

            // Toroidal boundary conditions: wrap around the edges.
            const int r = (row + dr + n) % n;
            const int c = (col + dc + n) % n;

            count += grid[r][c];
        }
    }

    return count;
}

void nextGeneration(const Grid& current, Grid& next, int n) {
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            const int neighbours = countNeighbours(current, row, col, n);

            if (current[row][col] == 1) {
                // A live cell survives with two or three live neighbours.
                next[row][col] =
                    (neighbours == 2 || neighbours == 3) ? 1 : 0;
            } else {
                // A dead cell becomes alive with exactly three neighbours.
                next[row][col] = (neighbours == 3) ? 1 : 0;
            }
        }
    }
}

Grid createGrid(int n, unsigned int seed, int liveProbability) {
    if (n < 1) {
        throw invalid_argument("Grid size must be positive.");
    }

    if (liveProbability < 0 || liveProbability > 100) {
        throw invalid_argument(
            "Live-cell probability must be between 0 and 100."
        );
    }

    Grid grid(n, vector<int>(n, 0));

    mt19937 generator(seed);
    uniform_int_distribution<int> distribution(0, 99);

    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            grid[row][col] =
                (distribution(generator) < liveProbability) ? 1 : 0;
        }
    }

    return grid;
}

Grid createBlinkerPattern(int n) {
    if (n < 5) {
        throw invalid_argument(
            "Blinker pattern requires a grid of at least 5 x 5."
        );
    }

    Grid grid(n, vector<int>(n, 0));

    const int center = n / 2;

    grid[center - 1][center] = 1;
    grid[center][center] = 1;
    grid[center + 1][center] = 1;

    return grid;
}

long long countLiveCells(const Grid& grid) {
    long long count = 0;

    for (const auto& row : grid) {
        for (const int cell : row) {
            count += cell;
        }
    }

    return count;
}

SimulationResult runSimulation(Grid current, int iterations) {
    if (iterations < 1) {
        throw invalid_argument("Iteration count must be positive.");
    }

    const int n = static_cast<int>(current.size());

    if (n < 3) {
        throw invalid_argument("Grid size must be at least 3.");
    }

    for (const auto& row : current) {
        if (static_cast<int>(row.size()) != n) {
            throw invalid_argument("Grid must be square.");
        }
    }

    Grid next(n, vector<int>(n, 0));

    // Use a monotonic clock and time only the simulation loop.
    const auto start = steady_clock::now();

    for (int i = 0; i < iterations; ++i) {
        nextGeneration(current, next, n);
        current.swap(next);
    }

    const auto end = steady_clock::now();

    const double elapsedMs =
        duration<double, milli>(end - start).count();

    SimulationResult result;

    // Preserve the actual final grid so it can be displayed or compared.
    result.finalGrid = std::move(current);
    result.finalLiveCells = countLiveCells(result.finalGrid);
    result.elapsedMs = elapsedMs;
    result.totalCellUpdates =
        1LL * n * n * iterations;

    result.cellUpdatesPerSecond =
        elapsedMs > 0.0
            ? result.totalCellUpdates / (elapsedMs / 1000.0)
            : 0.0;

    return result;
}

static void printGrid(const Grid& grid) {
    for (const auto& row : grid) {
        for (const int cell : row) {
            cout << (cell == 1 ? "O " : ". ");
        }

        cout << '\n';
    }
}

bool runBlinkerCorrectnessTest() {
    const int n = 5;

    Grid current = createBlinkerPattern(n);
    Grid next(n, vector<int>(n, 0));

    // After one generation, the vertical blinker must be horizontal.
    nextGeneration(current, next, n);
    current.swap(next);

    const long long firstGenLiveCells = countLiveCells(current);

    const bool afterOneGeneration =
        current[2][1] == 1 &&
        current[2][2] == 1 &&
        current[2][3] == 1 &&
        firstGenLiveCells == 3 &&
        current[1][2] == 0 &&
        current[3][2] == 0;

    // After two generations, it must be vertical again.
    nextGeneration(current, next, n);
    current.swap(next);

    const long long secondGenLiveCells = countLiveCells(current);

    const bool afterTwoGenerations =
        current[1][2] == 1 &&
        current[2][2] == 1 &&
        current[3][2] == 1 &&
        secondGenLiveCells == 3 &&
        current[2][1] == 0 &&
        current[2][3] == 0;

    return afterOneGeneration && afterTwoGenerations;
}

int main(int argc, char* argv[]) {
    try {
        // Run the correctness test independently of performance benchmarks.
        if (argc >= 2 &&
            (string(argv[1]) == "--blinker-test" ||
             string(argv[1]) == "--self-test")) {

            if (!runBlinkerCorrectnessTest()) {
                cerr << "Blinker correctness test failed.\n";
                return 1;
            }

            cout << "Blinker correctness test passed.\n";
            return 0;
        }

        int n = 5;
        int iterations = 100;
        bool showGrid = true;

        if (argc >= 2) {
            n = stoi(argv[1]);
        }

        if (argc >= 3) {
            iterations = stoi(argv[2]);
        }

        if (argc >= 4) {
            const string displayArgument = argv[3];

            if (displayArgument == "show") {
                showGrid = true;
            } else if (displayArgument == "hide") {
                showGrid = false;
            } else {
                throw invalid_argument(
                    "Display argument must be 'show' or 'hide'."
                );
            }
        }

        if (n < 3) {
            throw invalid_argument(
                "Grid size must be at least 3."
            );
        }

        if (iterations < 1) {
            throw invalid_argument(
                "Iteration count must be positive."
            );
        }

        // Official benchmark initialization: fixed seed, 30% live probability.
        Grid initialGrid = createGrid(n, 42, 30);

        if (showGrid) {
            cout << "Initial grid (" << n << " x " << n << "):\n";
            printGrid(initialGrid);
        }

        const SimulationResult result =
            runSimulation(initialGrid, iterations);

        cout << fixed << setprecision(3);

        cout << "\n========== CPU RESULTS ==========\n";
        cout << "Grid size: " << n << " x " << n << '\n';
        cout << "Iterations: " << iterations << '\n';
        cout << "Random seed: 42\n";
        cout << "Initial live-cell probability: 30%\n";
        cout << "Boundary condition: Toroidal (wraparound)\n";
        cout << "Final live cells: "
             << result.finalLiveCells << '\n';
        cout << "Simulation time: "
             << result.elapsedMs << " ms\n";
        cout << "Total cell updates: "
             << result.totalCellUpdates << '\n';
        cout << "Cell updates per second: "
             << result.cellUpdatesPerSecond << '\n';
        cout << "=================================\n";

        if (showGrid) {
            cout << "\nFinal grid:\n";
            printGrid(result.finalGrid);
        }

        return 0;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        cerr << "Usage:\n";
        cerr << "  game_of_life_cpu.exe [grid_size] [iterations] [show|hide]\n";
        cerr << "  game_of_life_cpu.exe --blinker-test\n";
        return 1;
    }
}
