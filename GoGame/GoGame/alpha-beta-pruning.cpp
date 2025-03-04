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
#include <cstdlib>

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
bool moveCheck(int row, int col, int player, std::shared_ptr<Node> node, set<pair<int, int>>& visited) {
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
int removeStones(int row, int col, int player, std::shared_ptr<Node> node) {
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
int checkCaptures(int opponent, std::shared_ptr<Node> node) {
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
   
    return captures;
}

// Helper function to count liberties of a given stone
int countLiberties(int row, int col, int player, std::shared_ptr<Node> node, set<pair<int, int>>& visited) {
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

int calculateHeatmapValue(int x, int y, int boardSize) {
    // Check if the move is on the edge
    if (x == 0 || x == boardSize - 1 || y == 0 || y == boardSize - 1) {
        return 10; // Edge (lowest value)
    }

    // Distance from the edges
    int distX = std::min(x, boardSize - 1 - x);
    int distY = std::min(y, boardSize - 1 - y);
    int minDist = std::min(distX, distY);

    // Assign values based on distance from the edge
    if (minDist == 1) {
        return 25; // One line away from edge
    }
    else if (minDist == 2 || minDist == 3) {
        return 40; // Two lines away from edge (optimal area)
    }
    else if (minDist > 3) {
        return 30; // Center (semi-good)
    }

    return 10; // Default (should not happen)
}

int calculateConnectionBonus(int x, int y, int boardSize, const std::vector<std::vector<int>>& board, int currentPlayer) {
    int connectionBonus = 0;
    bool noteye = false;

    // Check all 8 neighboring cells
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue; // Skip the current cell
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize) {
                if (board[nx][ny] == currentPlayer) {
                    if (nx || ny == 0) connectionBonus + 15;
                    connectionBonus += 15;
                }// Add bonus for each connected friendly stone
                else noteye = true;
            }
        }
    }

    if (!noteye) connectionBonus -= 500;
    return connectionBonus;
}


// Added by Andrija Sevaljevic
// This function calcaultes the strength of a move
int evaluateBoard(int currentStone, std::shared_ptr<Node> node) {
    int score = 0;
    int currentTurnStone = currentStone;
    int goodStones = 0;
    int badStones = 0;
    set<pair<int, int>> visited;
    std::srand(std::time(0));

    // Capture score
    int capturedStones = checkCaptures(currentTurnStone * -1, node);
    score += capturedStones * (100 + (capturedStones * 5));
    node->captureValue = score;
    if (score == 0) {
        capturedStones = checkCaptures(currentTurnStone, node);
        score -= capturedStones * (100 + (capturedStones * 5));
        node->captureValue -= score;
    }

    // Evaluate liberties and weaknesses
    int liberties = 0;
    int weakStones = 0;
    int groupStrength = 0;
    int totalLiberties = 0;
    int topLeftInfluence = 0;
    int topRightInfluence = 0;
    int bottomLeftInfluence = 0;
    int bottomRightInfluence = 0;
    int connectionBonus = calculateConnectionBonus(node->moveX, node->moveY, node->boardSize, node->board, currentStone);

    for (int x = 0; x < node->boardSize; x++) {
        for (int y = 0; y < node->boardSize; y++) {
            if (node->board[x][y] == currentTurnStone) {
                goodStones++;

                if (x <= node->boardSize / 2 && y <= node->boardSize / 2) topLeftInfluence++;
                else if (x >= node->boardSize / 2 && y >= node->boardSize / 2) bottomRightInfluence++;
                else if (x >= node->boardSize / 2 && y <= node->boardSize / 2) topRightInfluence++;
                else bottomLeftInfluence++;

                if (!visited.count({ x, y })) {
                    int libertiesForStone = countLiberties(x, y, currentTurnStone, node, visited);
                    liberties += libertiesForStone;
                    totalLiberties += libertiesForStone;

                    if (libertiesForStone <= 2) {
                        weakStones++;
                    }

                    if (libertiesForStone == 1) score -= 300;
                }
            }
            else if (node->board[x][y] == currentTurnStone * -1) {
                badStones++;
            }
        }
    }

    groupStrength = totalLiberties / (node->boardSize);

    if (goodStones + badStones < node->boardSize * node->boardSize / 4) {
        // Early game: prioritize liberties
        liberties *= 20; // Increased weight on liberties
        groupStrength *= 5; // Lower weight on group strength
        if(std::rand() % 100 < 0.3) score += connectionBonus * 0.5;
        if((goodStones + badStones) < 13 && (goodStones + badStones) < node->boardSize) score += calculateHeatmapValue(node->moveX, node->moveY, node->boardSize);
    }
    else if (goodStones + badStones < node->boardSize * node->boardSize * 2 / 3) {
        // Mid game: balance between liberties and group strength
        liberties *= 8;
        groupStrength *= 10;
        if (std::rand() % 100 < 0.75) score += connectionBonus;
        if (connectionBonus == 0) score -= 10;
    }
    else {
        // Late game: prioritize group strength
        liberties *= 5;
        groupStrength *= 20; // Increased weight on group strength
        if (std::rand() % 100 < 0.9) score += connectionBonus;
        if (connectionBonus == 0) score -= 25;
    }

    score -= weakStones * 25; // Still penalizing weak stones
    node->weakStoneValue = weakStones * -25;

    score += liberties / (goodStones + badStones + 1);  // Increased weight on liberties
    node->libertyValue = liberties / (goodStones + badStones + 1);

    score += groupStrength;
    node->groupValue = groupStrength;

    score += (goodStones - badStones) * 100;
    node->stoneValue = (goodStones - badStones) * 100;

    int minInfluence = std::min({ topLeftInfluence, topRightInfluence, bottomLeftInfluence, bottomRightInfluence });
    int maxInfluence = std::max({ topLeftInfluence, topRightInfluence, bottomLeftInfluence, bottomRightInfluence });

    // Penalize if one quadrant is too dominant
    if (maxInfluence - minInfluence > 4) {
        score -= (maxInfluence - minInfluence) * 20; // Penalize imbalance
    }

    score += topLeftInfluence + topRightInfluence + bottomLeftInfluence + bottomRightInfluence;

    score = score * currentStone;
    node->value = score;

    

    return score;
}

// Added by Andrija Sevaljevic
// This function cycles through all of our possible moves
int alphaBeta(std::shared_ptr<Node> node, int depth, int alpha, int beta, bool maximizingPlayer, steady_clock::time_point startTime) {

    auto elapsed = duration_cast<seconds>(steady_clock::now() - startTime).count();
    if (elapsed >= 10) {
        return node->value;
    }

    if (depth == 0) {
        return node->value;
    }

    // Ensure children are generated for this node if not already done
    if (node->children.empty()) {
        generateNChildren(node, maximizingPlayer);
    }

    if (node->children.empty()) {
        return node->value; // If no children, return evaluation
    }

    int dynamicDepth = (node->children.size() > 20) ? depth - 2 : depth - 1;

    if (maximizingPlayer && depth != 0) {
        int maxEval = std::numeric_limits<int>::min();
        for (std::shared_ptr<Node> child : node->children) {

            auto elapsed = duration_cast<seconds>(steady_clock::now() - startTime).count();
            if (elapsed >= 10) {
                return node->value;
            }

            int eval = alphaBeta(child, dynamicDepth, alpha, beta, false, startTime);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Beta cut-off
        }
        return maxEval;
    }
    else {
        int minEval = std::numeric_limits<int>::max();
        for (std::shared_ptr<Node> child : node->children) {

            auto elapsed = duration_cast<seconds>(steady_clock::now() - startTime).count();
            if (elapsed >= 10) {
                return node->value;
            }

            int eval = alphaBeta(child, dynamicDepth, alpha, beta, true, startTime);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Alpha cut-off
        }
        return minEval;
    }
}

// Added by Andrija Sevaljevic
// This function generates a tree of all possible moves
void generateChildren(std::shared_ptr<Node> node, bool isMaximizing) {
    int playerValue = isMaximizing ? 1 : -1;  // Assign '1' to Player 1, '-1' to Player 2

    for (int x = 0; x < node->boardSize; x++) {
        for (int y = 0; y < node->boardSize; y++) {
            if (node->board[x][y] == 0) { // Empty spot
                std::vector<std::vector<int>> newBoard = node->board;
                newBoard[x][y] = playerValue;
                node->children.push_back(std::make_shared<Node>(newBoard, node->boardSize, x, y, 0, node));
            }
        }
    }
}

// Added by Andrija Sevaljevic
// This function generates a tree of N best possible moves
void generateNChildren(std::shared_ptr<Node> node, bool isMaximizing) {
    int playerValue = isMaximizing ? 1 : -1;  // Assign '1' to Player 1, '-1' to Player 2
    vector<pair<int, std::shared_ptr<Node>>> evaluatedChildren; // Pair of evaluation score and Node pointer

    // Generate all possible moves and evaluate them
    for (int x = 0; x < node->boardSize; x++) {
        for (int y = 0; y < node->boardSize; y++) {
            if (node->board[x][y] == 0) { // Empty spot
                std::vector<std::vector<int>> newBoard = node->board;
                newBoard[x][y] = playerValue;


                // Create the new node and add it to the list with its evaluation score
                std::shared_ptr<Node> childNode = std::make_shared<Node>(newBoard, node->boardSize, x, y, 0, node);
                int temp = evaluateBoard(isMaximizing, childNode);
                evaluatedChildren.push_back({ childNode->value, childNode });
            }
        }
    }

    if (isMaximizing) {
        // Sort children based on the evaluation score (highest first)
        sort(evaluatedChildren.begin(), evaluatedChildren.end(),
            [](const auto& a, const auto& b) {
                return a.first > b.first;
            });
    }
    else {
        // Sort children based on the evaluation score (lowest first)
        sort(evaluatedChildren.begin(), evaluatedChildren.end(),
            [](const auto& a, const auto& b) {
                return a.first < b.first;
            });
    }

    // Get the total number of generated moves
    int moveCount = evaluatedChildren.size();
    int topCount = min(5, moveCount);          // Top 5 moves
    int midCount = min(5, max(0, moveCount - 5)); // Middle 5 moves

    // Add top 5 moves
    for (int i = 0; i < topCount; i++) {
        node->children.push_back(evaluatedChildren[i].second);
    }

    // Add middle 5 moves (starting after the first 5)
    int midStartIndex = moveCount / 2;
    for (int i = midStartIndex; i < midStartIndex + midCount; i++) {
        if (i < moveCount) {
            node->children.push_back(evaluatedChildren[i].second);
        }
    }

    for(auto& element: evaluatedChildren){
        freeChildren(element.second->children);
    }
}


//Rhett Thompson
void freeChildren(std::vector<std::shared_ptr<Node>>& children){
    for(int i = 0; i < children.size(); i++){
        if(children[i] != nullptr){
            freeChildren( (children[i])->children);
        }
    }
    children.clear();
}

