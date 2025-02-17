// alpha-beta-pruning.h

#ifndef ALPHA_BETA_PRUNING_H
#define ALPHA_BETA_PRUNING_H

#include <vector>
#include <limits>
#include <chrono>
#include <algorithm>

using namespace std::chrono;

// File implemented by Andrija Sevaljevic
// This file is the declaration file for the min max algorithm
struct Node;

// Function declarations for alpha-beta pruning
void generateChildren(Node* node, bool isMaximizing);
int alphaBeta(Node* node, int depth, int alpha, int beta, bool maximizingPlayer, steady_clock::time_point startTime);
int evaluateBoard(const std::vector<std::vector<int>>& board);

// Creating node structure for algorithm
struct Node {
    std::vector<std::vector<int>> board;
    int moveX, moveY, value, boardSize;
    Node* parent;
    std::vector<Node*> children;

    Node(std::vector<std::vector<int>> board, int size, int moveX = -1, int moveY = -1, int value = 0, Node* parent = nullptr)
        : board(board), boardSize(size), moveX(moveX), moveY(moveY), value(value), parent(parent) {}
};

#endif
