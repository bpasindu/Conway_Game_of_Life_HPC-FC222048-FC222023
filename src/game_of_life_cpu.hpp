
#ifndef GAME_OF_LIFE_CPU_HPP
#define GAME_OF_LIFE_CPU_HPP

#include <vector>

using Grid = std::vector<std::vector<int>>;

struct SimulationResult {
    Grid finalGrid;
    long long finalLiveCells = 0;
    long long totalCellUpdates = 0;
    double elapsedMs = 0.0;
    double cellUpdatesPerSecond = 0.0;
};

int countNeighbours(const Grid& grid, int row, int col, int n);

void nextGeneration(const Grid& current, Grid& next, int n);

Grid createGrid(int n, unsigned int seed, int liveProbability);

Grid createBlinkerPattern(int n);

long long countLiveCells(const Grid& grid);

SimulationResult runSimulation(Grid current, int iterations);

bool runBlinkerCorrectnessTest();

#endif
