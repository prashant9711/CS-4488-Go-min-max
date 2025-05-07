//#include "pch.h"

#include <iostream>
#include <vector>
#include <cctype>
#include <sstream>
#include <limits>
#include <thread>
#include <chrono>
#include <limits>
#include <set>
#include <map>
#include <chrono>
#include <algorithm>
#include <mutex>
#include <future>

using namespace std::chrono;
std::mutex mtx;

#ifndef GO_WINDOW
#include "window.hpp"
#endif

#ifndef GO_BOARD
#include "board.hpp"
#endif

#ifndef ALPHA_BETA_PRUNING_H
#include "alpha-beta-pruning.h"
#endif

#ifndef GO_UTIL
#include "goutil.hpp"
#endif

//Comment this out if you want to use the unicode board
#define PRINT_WINDOW  //If this is defined, then Print_Window will be used


using namespace std;

// Created by Ethan
class GoGame {
private:
    // Initial variables for board and game creation
    std::shared_ptr<Board> board_class;
    std::shared_ptr<Window> window;
    std::string gameMode;
    int size;
    // Added by Prashant
    Space_Types currentPlayer;
    bool hasBot = false;
    int passCount; // To keep track of pass count of the players

    //edited by Prashant changed the first player to black
public:
    GoGame(int boardSize, const std::string& game_mode) : size(boardSize), gameMode(game_mode), currentPlayer(BLACK), passCount(0) {

        int screenWidth = 120;
        int screenHeight = 60;

        int distance_bw_top = 5;
        int distance_bw_side = 20;

        int boardWidth =  ( (screenWidth-distance_bw_side) / (size-1) ) * (size-1) + 1;
        int boardHeight = ( (screenHeight-distance_bw_top) / (size-1) ) * (size-1) + 1;

#ifndef PRINT_WINDOW
        board_class = std::make_shared<Wchar_Board>(
            boardWidth,
            boardHeight,
            size,
            (screenWidth - boardWidth) / 2,
            (screenHeight - boardHeight) / 2
        );
        window = std::make_shared<ConsoleWindow>(screenHeight, screenWidth, board_class);
#else
        board_class = std::make_shared<Char_Board>(
            boardWidth,
            boardHeight,
            size,
            (screenWidth - boardWidth) / 2,
            (screenHeight - boardHeight) / 2
        );
        window = std::make_shared<Print_Window>(screenHeight, screenWidth, board_class);
#endif

    }


    // Created by Prashant
    // Checking if the position is on the board
    bool onBoard(int row, int col) {
        return row >= 0 && row < size && col >= 0 && col < size;
    }

    // Getting the neighbors of the current position
    vector<pair<int, int>> getNeighbors(int row, int col) {
        return {
            {row - 1, col}, {row + 1, col},
            {row, col - 1}, {row, col + 1}
        };
    }

    // recursively checking for suicide moves, ko
    // Created by Prashant
    // Modified by Rhett
    bool moveCheck(int row, int col, Space_Types player, set<pair<int, int>>& visited) {
        const std::vector<std::vector<Space_Types>>& board = this->board_class->getStones();
        if (!onBoard(row, col) || visited.count({ row, col }) || board[row][col] != player) {
            return false;
        }
        visited.insert({ row, col }); //checking whether the current index has been visit previously or not

        for (const auto& neighbor : getNeighbors(row, col)) {
            int r = neighbor.first;
            int c = neighbor.second;
            if (onBoard(r, c)) {
                if (board[r][c] == EMPTY) return true;
                if (board[r][c] == player && moveCheck(r, c, player, visited)) return true;
            }
        }
        return false;
    }

    // remove the captured stones
    // Created by Prashant
    // Modified by Rhett
    void removeStones(int row, int col, Space_Types player) {
        std::vector<std::vector<Space_Types>>& board = this->board_class->getStones();
        board[row][col] = EMPTY;
        for (const auto& neighbor : getNeighbors(row, col)) {
            int r = neighbor.first;
            int c = neighbor.second;
            if (onBoard(r, c) && board[r][c] == player) {
                removeStones(r, c, player);
            }
        }
    }

    // created by Andrija
    void printSummary(std::shared_ptr<Node> node) {
        cout << "Capture value: " << node->captureValue << "\n";
        cout << "Liberty value: " << node->libertyValue << "\n";
        cout << "Group value: " << node->groupValue << "\n";
        cout << "Weak stone value: " << node->weakStoneValue << "\n";
        cout << "Stone value: " << node->stoneValue << "\n";
        cout << "Total value: " << node->value << "\n";
    }

    // Created by Prashant
    // Modified by Rhett
    // check for captures and remove
    void checkCaptures(int row, int col) {
        //char opponent = (currentPlayer == 'W') ? 'B' : 'W';
        std::vector<std::vector<Space_Types>>& board = this->board_class->getStones();
        Space_Types opponent = static_cast<Space_Types>(!static_cast<bool>(currentPlayer));

        for (const auto& neighbor : getNeighbors(row, col)) {
            int r = neighbor.first;
            int c = neighbor.second;
            if (onBoard(r, c) && board[r][c] == opponent) {
                set<pair<int, int>> visited;
                if (!moveCheck(r, c, opponent, visited)) {
                    for (const auto& stone : visited) {
                        removeStones(stone.first, stone.second, opponent);
                    }
                }
            }
        }
    }

    // Created by Ethan
    // Modified by Rhett
    // Modified by Prashant
    bool placeStone(int row, int col) { //Using 0 based indexing here

        if (row == -100 && col == -100) { //I'm using these values to represent a pass
            passCount++;
            std::cout << "Player: " << currentPlayer << " passed\n";
            return true;
        }
        passCount = 0;
        bool result = this->board_class->place_stone_on_board(row, col, currentPlayer);
        if (!result) return result;

        // Added by Prashant
        checkCaptures(row, col); // Check for captures

        std::vector<std::vector<Space_Types>>& board = this->board_class->getStones();
        set<pair<int, int>> visited;
        //Check for illegal moves
        if (!moveCheck(row, col, currentPlayer, visited)) {
            board[row][col] = EMPTY;
            return false; // Illegal suicide move
        }

        return result;
    }


    // Added by Prashant
    // Modified by Rhett
    // total score= n of placed stones+captured territory
    void calculateScores() {
        // initializing scores
        std::vector<std::vector<Space_Types>>& board = this->board_class->getStones();
        int whiteScore = 0, blackScore = 0;
        map<char, int> stoneCount = { {'W', 0}, {'B', 0} };
        vector<vector<bool>> visited(size, vector<bool>(size, false));

        // Count placed stones
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == WHITE) stoneCount['W']++;
                if (board[row][col] == BLACK) stoneCount['B']++;
            }
        }

        // Checking for captured territory and their owners
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == EMPTY && !visited[row][col]) {
                    set<pair<int, int>> territory;
                    set<Space_Types> surroundingColors;
                    territoryCheck(row, col, territory, surroundingColors, visited);

                    if (surroundingColors.size() == 1) {
                        Space_Types owner = *surroundingColors.begin();
                        if (owner == WHITE) whiteScore += static_cast<int>(territory.size());
                        else if (owner == BLACK) blackScore += static_cast<int>(territory.size());
                    }
                }
            }
        }
        // Adding the stone count to the score
        whiteScore += stoneCount['W'];
        blackScore += stoneCount['B'];

        // Printing the results
        cout << "\nGame Over! Final Scores:\n";
        cout << "White (W): " << whiteScore << "\n";
        cout << "Black (B): " << blackScore << "\n";

        // Checking for winner
        if (whiteScore > blackScore)
            cout << "White Wins!\n";
        else if (blackScore > whiteScore)
            cout << "Black Wins!\n";
        else
            cout << "It's a tie!\n";
    }

    //Added by Prashant
    // Modified by Rhett
    //  recursively checking for empty regions and determining their ownership
    void territoryCheck(int row, int col, set<pair<int, int>>& territory, set<Space_Types>& surroundingColors, vector<vector<bool>>& visited) {
        std::vector<std::vector<Space_Types>>& board = this->board_class->getStones();

        if (!onBoard(row, col) || visited[row][col]) return;
        visited[row][col] = true;
        if (board[row][col] == EMPTY) {
            territory.insert({ row, col });
            for (const auto& neighbor : getNeighbors(row, col)) {
                territoryCheck(neighbor.first, neighbor.second, territory, surroundingColors, visited);
            }
        }
        else {
            surroundingColors.insert(board[row][col]);
        }
    }

    //Added by Prashant
    // Modified by Rhett
    // func to allow the bot to make random moves
    bool botMove() {
        std::vector<std::vector<Space_Types>>& board = this->board_class->getStones();
        vector<pair<int, int>> emptySpaces;
        srand(time(0));
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == EMPTY) {
                    emptySpaces.push_back({ row, col });
                }
            }
        }

        if (emptySpaces.empty()) return false; // No valid moves left (pass)

        // Randomly select a move
        int randomIndex = rand() % emptySpaces.size();
        int row = emptySpaces[randomIndex].first;
        int col = emptySpaces[randomIndex].second;

        // Place the bot's stone
        bool result = this->board_class->place_stone_on_board(row, col, currentPlayer);

        // Checking for captures
        checkCaptures(row, col);

        set<pair<int, int>> visited;
        // stopping illegal moves
        if (!moveCheck(row, col, currentPlayer, visited)) {
            board[row][col] = EMPTY;
            return false;
        }

        return true;
    }

    // added by Andrija Sevaljevic
    bool alphaBetaMove() {
        std::vector<std::vector<Space_Types>>& board = this->board_class->getStones();
        vector<pair<int, int>> emptySpaces;

        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == EMPTY) {
                    emptySpaces.push_back({ row, col });
                }
            }
        }

        if (emptySpaces.empty()) return false; // No valid moves left (pass)

        // Convert char board to int board (-1 = black, 1 = white, 0 = empty)
        vector<vector<int>> intBoard(size, vector<int>(size, 0));
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (board[i][j] == BLACK) intBoard[i][j] = -1;
                else if (board[i][j] == WHITE) intBoard[i][j] = 1;
            }
        }

        // Create root node
        std::shared_ptr<Node> root = std::make_shared<Node>(intBoard, size);

        // Generate possible moves
        generateNChildren(root, (currentPlayer == WHITE));

        if (root->children.empty()) {
            // No possible moves
            return false;
        }

        // Run alpha-beta pruning in parallel
        int bestValue = -10000;
        std::shared_ptr<Node> bestMove = nullptr;
        // Function to evaluate a subset of child nodes
        auto evaluateChildren = [&](int start, int end) {

            int localBestValue = -10000;
            std::shared_ptr<Node> localBestMove = nullptr;

            for (int i = start; i < end; i++) {
                auto startTime = steady_clock::now();
                int eval = alphaBeta(root->children[i], 3, -10000, 10000, false, startTime);
                if (eval > localBestValue) {
                    localBestValue = eval;
                    localBestMove = root->children[i];
                }
            }
            //Added by Prashant
            // Lock the mutex to update the shared bestValue and bestMove
            std::lock_guard<std::mutex> lock(mtx);
            if (localBestValue > bestValue) {
                bestValue = localBestValue;
                bestMove = localBestMove;
            }
        };


        //Bandaid Fix (added by Rhett)
        std::set<std::shared_ptr<Node>> unique_nodes = {};
        auto it = root->children.begin();
        while (it != root->children.end()) {
            if (unique_nodes.find(*it) != unique_nodes.end()) {
                it = root->children.erase(it);
                continue;
            }
            unique_nodes.insert(*it);
            it++;
        }
        //----------------------------------------------

        //Added by Prashant
        // Modified by Rhett
        // Determine the number of threads to use
        unsigned int numThreads = root->children.size();
        int childrenPerThread = root->children.size() / numThreads;
        if (childrenPerThread <= 0) childrenPerThread = 1;
        int start = 0;
        int end = childrenPerThread;
        std::vector<std::thread> threads;


        for (; start < numThreads * childrenPerThread; start += childrenPerThread, end += childrenPerThread) {
            if (start == (numThreads * childrenPerThread) - childrenPerThread) end = root->children.size();
            threads.emplace_back(evaluateChildren, start, end);
        }

        // Wait for all threads to finish
        //Added by Prashant
        for (auto& thread : threads) {
            thread.join();
        }


        freeChildren(root->children);  // Free memory

        if (bestMove) {
            int row = bestMove->moveX;
            int col = bestMove->moveY;
            board[row][col] = currentPlayer;

            // Check captures
            checkCaptures(row, col);

            printSummary(bestMove);

            // Prevent illegal moves
            set<pair<int, int>> visited;
            if (!moveCheck(row, col, currentPlayer, visited)) {
                board[row][col] = EMPTY;  // Undo move
                return false;
            }
        }

        return true;
    }

    bool alphaBetaMove2() {

        std::vector<std::vector<Space_Types>>& board = this->board_class->getStones();
        vector<pair<int, int>> emptySpaces;

        // Find all empty spaces on the board
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == EMPTY) {
                    emptySpaces.push_back({ row, col });
                }
            }
        }

        if (emptySpaces.empty()) return false; // No valid moves left (pass)

        // Convert char board to int board (-1 = black, 1 = white, 0 = empty)
        vector<vector<int>> intBoard(size, vector<int>(size, 0));
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (board[i][j] == WHITE) intBoard[i][j] = -1; // Swap roles of 'W' and 'B'
                else if (board[i][j] == BLACK) intBoard[i][j] = 1;
            }
        }

        // Create root node
        std::shared_ptr<Node> root = std::make_shared<Node>(intBoard, size);

        // Generate possible moves
        generateNChildren(root, (currentPlayer == BLACK)); // Swap roles for player

        if (root->children.empty()) {
            // No possible moves
            return false;
        }

        // Run alpha-beta pruning in parallel
        int bestValue = -10000;
        std::shared_ptr<Node> bestMove = nullptr;
        // Function to evaluate a subset of child nodes
        auto evaluateChildren = [&](int start, int end) {

            int localBestValue = -10000;
            std::shared_ptr<Node> localBestMove = nullptr;

            for (int i = start; i < end; i++) {
                auto startTime = steady_clock::now();
                int eval = alphaBeta(root->children[i], 3, -10000, 10000, false, startTime);
                if (eval > localBestValue) {
                    localBestValue = eval;
                    localBestMove = root->children[i];
                }
            }

            // Lock the mutex to update the shared bestValue and bestMove
            //Added by Prashant
            std::lock_guard<std::mutex> lock(mtx);
            if (localBestValue > bestValue) {
                bestValue = localBestValue;
                bestMove = localBestMove;
            }
        };
        //Bandaid fix (added by Rhett).
        std::set<std::shared_ptr<Node>> unique_nodes = {};
        auto it = root->children.begin();
        while (it != root->children.end()) {
            if (unique_nodes.find(*it) != unique_nodes.end()) {
                it = root->children.erase(it);
                continue;
            }
            unique_nodes.insert(*it);
            it++;
        }
        //----------------------------------

        // Determine the number of threads to use
        //Added by Prashant
        unsigned int numThreads = root->children.size();

        int childrenPerThread = root->children.size() / numThreads;
        if (childrenPerThread <= 0) childrenPerThread = 1;

        // Create threads
        //Added by Prashant
        //Modified by Rhett
        int start = 0;
        int end = childrenPerThread;
        std::vector<std::thread> threads;

        for (; start < numThreads * childrenPerThread; start += childrenPerThread, end += childrenPerThread) {
            if (start == (numThreads * childrenPerThread) - childrenPerThread) end = root->children.size();
            threads.emplace_back(evaluateChildren, start, end);
        }

        // Wait for all threads to finish
        //Added by Prashant
        for (auto& thread : threads) {
            thread.join();
        }


        freeChildren(root->children);  // Free memory

        if (bestMove) {
            int row = bestMove->moveX;
            int col = bestMove->moveY;
            board[row][col] = currentPlayer;

            // Check captures
            checkCaptures(row, col);

            // Print summary of the move
            printSummary(bestMove);

            // Prevent illegal moves
            set<pair<int, int>> visited;
            if (!moveCheck(row, col, currentPlayer, visited)) {
                board[row][col] = EMPTY;  // Undo move
                return false;
            }

        }

        return true;
    }

    // Gameplay loop
    // Created by Ethan
    // Modified by Rhett
    void play() {


        std::unique_ptr<std::pair<float, float>> input_coords;
        window->clear();
        

        while (1) {

            window->display();

            if (currentPlayer == WHITE) {
                cout << "Bot (" << currentPlayer << ") is making a move...\n";
                if (!alphaBetaMove2()) { // Use parallelized alphaBetaMove2
                    cout << "No valid moves left for the bot. Passing turn.\n";
                    passCount++;
                }
            }
            else {
                if (gameMode == "2") {
                    cout << "Bot (" << currentPlayer << ") is making a move...\n";
                    if (!alphaBetaMove()) { // Use parallelized alphaBetaMove
                        cout << "No valid moves left for the bot. Passing turn.\n";
                        passCount++;
                    }
                }
               else {
                    while (1) {
                        input_coords = window->get_input();
                        if (this->placeStone(static_cast<int>(input_coords->first), static_cast<int>(input_coords->second))) break;
                        std::cout << "Invalid move\n";
                    }
                }
            }
            
            window->clear();

            currentPlayer = static_cast<Space_Types>(!static_cast<bool>(currentPlayer));

            if (passCount >= 2) { //if both players pass the game ends
                window->display();
                window.reset();
                cout << "Both players passed. Game over!\n";
                calculateScores(); //getting the scores
                return;
            }
        }
    }


    //Rhett Thompson
    /**
      * Method for player vs player
      */
    void play_PVP() {

        std::unique_ptr<std::pair<float, float>> input_coords;
        window->clear();
        while (1) {
            window->display();
            while (1) {
                input_coords = window->get_input();
                if (this->placeStone(static_cast<int>(input_coords->first), static_cast<int>(input_coords->second))) break;
                std::cout << "Invalid move\n";
            }

            checkCaptures(static_cast<int>(input_coords->first), static_cast<int>(input_coords->second));
            window->clear();

            currentPlayer = static_cast<Space_Types>(!static_cast<bool>(currentPlayer));

            if (passCount >= 2) { //if both players pass the game ends
                window->display();
                window.reset();
                cout << "Both players passed. Game over!\n";
                calculateScores(); //getting the scores
                return;
            }
        }
    }


};

// Created by Ethan
    //Modified by Rhett
void mainMenu() {
    while (true) {
        // Initial game menu
        cout << "\n===== Go Game Menu =====\n";
        cout << "1. Start a new game\n";
        cout << "2. Quit\n";
        cout << "Enter choice: ";

            int choice = 0;

        while (1) {
            try {
                choice = std::stoi(Go_Util::get_keyboard_input());
                if (choice == 1 || choice == 2) break;
                std::cout << "Please enter 1 or 2:";

            }
            catch (std::invalid_argument err) {
                cout << "Invalid input! Please enter 1 or 2: ";
            }
        }

        if (choice == 2) {
            // Exit game from menu
            cout << "Exiting game. Goodbye!\n";
            return;
        }
        else if (choice == 1) {
            // Start game loop
            int boardSize;
            std::cout << "Choose the board size (5 <= size <= 19): ";
            while (1) {
                try {
                    boardSize = std::stoi(Go_Util::get_keyboard_input());
                    if (boardSize >= 5 && boardSize <= 19) break;
                    std::cout << "Please enter a number in between 5 and 19 (inclusive): ";

                }
                catch (std::invalid_argument err) {
                    cout << "Invalid input! Please enter a number in between 5 and 19 (inclusive): ";
                }
            }

            std::string gameMode;

            cout << "Choose game mode (1 for Player vs Bot, 2 for Bot vs Bot, 3 for Player vs Player): ";
            while (1) {
                gameMode = Go_Util::get_keyboard_input(1);
                if (gameMode == "1" || gameMode == "2" || gameMode == "3") break;
                cout << "Invalid choice! Please enter 1 for Player vs Bot, 2 for Bot vs Bot, or 3 for Player vs Player: ";
            }

            // Draw board and start gameplay loop
            GoGame game(boardSize, gameMode);
            if (gameMode == "3") {
                game.play_PVP();
                continue;
            }
            game.play();
        }
        else {
            // Error catch for invalid input
            cout << "Invalid choice! Please enter 1 or 2.\n";
        }
    }

}

// Created by Ethan
int main() {
    mainMenu();


    return 0;
}

