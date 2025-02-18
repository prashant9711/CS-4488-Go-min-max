// File implemented by Andrija Sevaljevic
// This file is used for our min max algorithm simulation

#include <iostream>
#include <vector>
#include <limits>
#include "alpha-beta-pruning.h"
#include <chrono>
#include <algorithm>
#include <set>
#include <vector>
#include <unordered_set>

using namespace std;
using namespace std::chrono;

// Getting the neighbors of the current position
vector<pair<int, int>> getNeighbors(int row, int col) {
    return {
        {row - 1, col}, {row + 1, col},
        {row, col - 1}, {row, col + 1}
    };
}

// Created by Prashant
// Modified to match data structure by Andrija Sevaljevic
// Checking if the position is on the board
bool onBoard(int row, int col, int size) {
    return row >= 0 && row < size&& col >= 0 && col < size;
}

// recursively checking for suicide moves, ko
// Modified for algorithm by Andrija Sevaljevic
// Created by Prashant
bool moveCheck(int row, int col, int player, Node* node, set<pair<int, int>>& visited) {
    if (!onBoard(row, col, node->boardSize) || visited.count({ row, col }) || node->board[row][col] != player) {
        return false; // Invalid move
    }

    visited.insert({ row, col }); // Mark current index as visited

    for (const auto& neighbor : getNeighbors(row, col)) {
        int r = neighbor.first;
        int c = neighbor.second;
        if (onBoard(r, c, node->boardSize)) {
            if (node->board[r][c] == 0) return true; // Found an empty spot
            if (node->board[r][c] == player && moveCheck(r, c, player, node, visited)) return true; // Continue searching if it's the same player
        }
    }
    return false; // No valid moves found
}

// remove the captured stones
// Modified to match data structure by Andrija Sevaljevic
// Created by Prashant
int removeStones(int row, int col, int player, Node* node) {
    node->board[row][col] = 0;
    for (const auto& neighbor : getNeighbors(row, col)) {
        int r = neighbor.first;
        int c = neighbor.second;
        if (onBoard(r, c, node->boardSize) && node->board[r][c] == player) {
            return 1 + removeStones(r, c, player, node);
        }
    }
    return 1;
}

// Created by Prashant
// Modified to match data structure by Andrija Sevaljevic
// check for captures and remove
int checkCaptures(bool currentPlayer, Node* node) {
    int opponent = (currentPlayer) ? 1 : -1;
    int captures = 0;
    int row = node->moveX;
    int col = node->moveY;

    for (const auto& neighbor : getNeighbors(row, col)) {
        int r = neighbor.first;
        int c = neighbor.second;
        if (onBoard(r, c, node->boardSize) && node->board[r][c] == opponent) {

            set<pair<int, int>> visited;
            if (!moveCheck(r, c, opponent, node, visited)) {
                for (const auto& stone : visited) {
                    captures += removeStones(stone.first, stone.second, opponent, node);
                }
            }
        }
    }
    captures = (currentPlayer) ? captures : captures * -1;
    return captures;
}

// Helper function to count liberties of a given stone
int countLiberties(int row, int col, int player, Node* node, set<pair<int, int>>& visited) {
    if (!onBoard(row, col, node->boardSize) || visited.count({ row, col }) || node->board[row][col] != player) {
        return 0;
    }

    visited.insert({ row, col });
    int liberties = 0;

    for (const auto& neighbor : getNeighbors(row, col)) {
        int r = neighbor.first;
        int c = neighbor.second;
        if (onBoard(r, c, node->boardSize)) {
            if (node->board[r][c] == 0) {
                liberties++;  // Empty spot is a liberty
            }
            else if (node->board[r][c] == player) {
                liberties += countLiberties(r, c, player, node, visited);  // Recursive count for group liberties
            }
        }
    }
    return liberties;
}


// Added by Andrija Sevaljevic
// This function calcaultes the strength of a move
int evaluateBoard(bool currentTurn, Node* node) {
    int score = 0;
    int currentTurnStone = currentTurn ? -1 : 1;

    // Capture score
    score += checkCaptures(currentTurn, node) * 300;
    if(score == 0) score += checkCaptures(!currentTurn, node) * 300;

    // Evaluate liberties and weaknesses
    int liberties = 0;
    int weakStones = 0;
    int groupStrength = 0;
    int totalLiberties = 0;

    for (int x = 0; x < node->boardSize; ++x) {
        for (int y = 0; y < node->boardSize; ++y) {
            if (node->board[x][y] == currentTurnStone) {  // Player's stones
                // Count liberties for each stone
                set<pair<int, int>> visited;
                int libertiesForStone = countLiberties(x, y, currentTurnStone, node, visited);
                liberties += libertiesForStone;
                totalLiberties += libertiesForStone;

                // Check if the stone is in a weak group (few liberties)
                if (libertiesForStone <= 2) {
                    weakStones++;
                }
            }
        }
    }

    // Penalize for weak stones (low liberties)
    score -= weakStones * 5;  // Arbitrary penalty for weak stones

    // Encourage strong groups (high liberties)
    score += liberties * 15;  // Encourage more liberties per stone

    // For simplicity, group strength could be based on liberties
    groupStrength = totalLiberties / (node->boardSize * node->boardSize);
    score += groupStrength * 10;

    if (currentTurn) score = score * -1;  // Reverse score if it's the maximizing player

    return score;
}

// Added by Andrija Sevaljevic
// This function cycles through all of our possible moves
int alphaBeta(Node* node, int depth, int alpha, int beta, bool maximizingPlayer, steady_clock::time_point startTime) {

    auto elapsed = duration_cast<seconds>(steady_clock::now() - startTime).count();
    if (elapsed >= 10) {
        return evaluateBoard(maximizingPlayer, node); // Return evaluation if time is up
    }

    // Evaluate at every step
    int currentEval = evaluateBoard(maximizingPlayer, node);

    if (depth == 0) {
        return currentEval;
    }

    // Ensure children are generated for this node if not already done
    if (node->children.empty()) {
        generateNChildren(node, maximizingPlayer);
    }

    if (node->children.empty()) {
        return currentEval; // If no children, return evaluation
    }

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (Node* child : node->children) {
            int eval = alphaBeta(child, depth - 1, alpha, beta, false, startTime);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Beta cut-off
        }
        return maxEval;
    }
    else {
        int minEval = std::numeric_limits<int>::max();
        for (Node* child : node->children) {
            int eval = alphaBeta(child, depth - 1, alpha, beta, true, startTime);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Alpha cut-off
        }
        return minEval;
    }
}

// Added by Andrija Sevaljevic
// This function generates a tree of all possible moves
void generateChildren(Node* node, bool isMaximizing) {
    int playerValue = isMaximizing ? 1 : -1;  // Assign '1' to Player 1, '-1' to Player 2

    for (int x = 0; x < node->boardSize; ++x) {
        for (int y = 0; y < node->boardSize; ++y) {
            if (node->board[x][y] == 0) { // Empty spot
                std::vector<std::vector<int>> newBoard = node->board;
                newBoard[x][y] = playerValue;
                node->children.push_back(new Node(newBoard, node->boardSize, x, y, 0, node));
            }
        }
    }
}

// Added by Andrija Sevaljevic
// This function generates a tree of N best possible moves
void generateNChildren(Node* node, bool isMaximizing) {
    int playerValue = isMaximizing ? 1 : -1;  // Assign '1' to Player 1, '-1' to Player 2
    vector<pair<int, Node*>> evaluatedChildren; // Pair of evaluation score and Node pointer

    // Generate all possible moves and evaluate them
    for (int x = 0; x < node->boardSize; ++x) {
        for (int y = 0; y < node->boardSize; ++y) {
            if (node->board[x][y] == 0) { // Empty spot
                std::vector<std::vector<int>> newBoard = node->board;
                newBoard[x][y] = playerValue;


                // Create the new node and add it to the list with its evaluation score
                Node* childNode = new Node(newBoard, node->boardSize, x, y, 0, node);
                childNode->value = evaluateBoard(isMaximizing, childNode);
                evaluatedChildren.push_back({ childNode->value, childNode });
            }
        }
    }

    // Sort children based on the evaluation score (highest first)
    std::partial_sort(evaluatedChildren.begin(),
        evaluatedChildren.begin() + std::min(10, (int)evaluatedChildren.size()),
        evaluatedChildren.end(),
        [](const auto& a, const auto& b) {
            return a.first > b.first;
        });

    // Add the top 10 (or fewer if there are not enough) to the node's children list
    for (int i = 0; i < std::min(10, (int)evaluatedChildren.size()); ++i) {
        node->children.push_back(evaluatedChildren[i].second);
    }
}

