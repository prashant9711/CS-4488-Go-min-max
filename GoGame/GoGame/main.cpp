//#include "pch.h"

#include <iostream>
#include <vector>
#include <cctype>
#include <sstream>
#include <limits>
#include <thread>
#include <chrono>

static constexpr uint32_t THREADS = 4;

 #ifndef GO_WINDOW
 #include "window.hpp"
 #endif

#ifndef GO_BOARD
#include "board.hpp"
#endif


using namespace std;

// Created by Ethan
class GoGame {
private:
    // Initial variables for board and game creation
    int size;
    vector<vector<char>> board;
    char currentPlayer;

public:
    GoGame(int boardSize) : size(boardSize), board(boardSize, vector<char>(boardSize, '.')), currentPlayer('W') {}

    // Temp display for board
    void displayBoard() {
        cout << "   ";
        for (int col = 0; col < size; col++) {
            cout << col << " "; // Build columns of board
        }
        cout << endl;

        for (int row = 0; row < size; row++) {
            cout << char('A' + row) << "  "; // Build rows of board
            for (int col = 0; col < size; col++) {
                cout << board[row][col] << " ";
            }
            cout << endl;
        }
    }

    // Created by Ethan
    bool placeStone(string move) {
        if (move == "quit") return false; // Quit to menu

        if (move.length() < 2) return false; // Invalid input check

        char rowChar = toupper(move[0]); // Convert row letter to uppercase
        int row = rowChar - 'A';

        // Convert the column number safely
        int col;
        stringstream ss(move.substr(1));
        if (!(ss >> col)) return false;  // Ensure conversion is valid

        if (row < 0 || row >= size || col < 0 || col >= size || board[row][col] != '.') {
            return false; // Catch input off board
        }

        board[row][col] = currentPlayer; // Set current player stone location
        currentPlayer = (currentPlayer == 'W') ? 'B' : 'W';  // Swap player
        return true;
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

        // Determine the number of threads to use
        unsigned int numThreads = std::thread::hardware_concurrency();
        int childrenPerThread = root->children.size() / numThreads;

        // Create threads
        //Added by Prashant
        std::vector<std::thread> threads;
        for (unsigned int i = 0; i < numThreads; i++) {
            int start = i * childrenPerThread;
            int end = (i == numThreads - 1) ? root->children.size() : start + childrenPerThread;
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

        // Determine the number of threads to use
        //Added by Prashant
        unsigned int numThreads = std::thread::hardware_concurrency();
        int childrenPerThread = root->children.size() / numThreads;

        // Create threads
        //Added by Prashant
        std::vector<std::thread> threads;
        for (unsigned int i = 0; i < numThreads; i++) {
            int start = i * childrenPerThread;
            int end = (i == numThreads - 1) ? root->children.size() : start + childrenPerThread;
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
    void play() {
        string move;
        while (true) {
            displayBoard();
            cout << "Player " << currentPlayer << ", enter move (e.g., A0) or type 'quit' to return to menu: ";
            cin >> move;

            if (!placeStone(move)) {
                if (move == "quit") return; // Quit game and return to menu mid game
                cout << "Invalid move! Try again.\n";
            }
        }
    }
};

// Created by Ethan
void mainMenu() {
    while (true) {
        // Initial game menu
        cout << "\n===== Go Game Menu =====\n";
        cout << "1. Start a new game\n";
        cout << "2. Quit\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        if (cin.fail()) {
            // Clear the error state and ignore invalid input
            cin.clear();
            //cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter 1 or 2.\n";
            continue;
        }

        if (choice == 2) {
            // Exit game from menu
            cout << "Exiting game. Goodbye!\n";
            return;
        }
        else if (choice == 1) {
            // Start game loop
            int boardSize;
            cout << "Choose board size (9, 13, 19): ";
            cin >> boardSize;

            // Build board from chosen size
            if (cin.fail() || (boardSize != 9 && boardSize != 13 && boardSize != 19)) {
                cin.clear();
                //cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid board size. Defaulting to 9x9.\n";
                boardSize = 9;
            }

            // Draw board and start gameplay loop
            GoGame game(boardSize);
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
    //mainMenu();

    
    int screenWidth = 120;
    int screenHeight = 60;
    
    int stoneField_size = 9;
    int boardWidth = ((stoneField_size - 1) * ((screenWidth / (stoneField_size - 1)) - 2)) + 1;
    int boardHeight = ((stoneField_size - 1) * ((screenHeight / (stoneField_size - 1)) - 1)) + 1;
    std::shared_ptr<Board> board = std::make_shared<Board>(
        boardWidth,
        boardHeight,
        stoneField_size,
        (screenWidth - boardWidth) / 2,
        (screenHeight - boardHeight) / 2
    );
    ConsoleWindow window(screenHeight, screenWidth, board);

    Space_Types turn = WHITE;
    window.display();
    while(1){

        while (!window.place_stone_for_player(turn)) {} //It would be better to have the board be the only one to place stones.
                                                        //Problem is, it depends on mouse input.  I will figure this out later.
        window.display();
        window.clear();
        
        std::this_thread::sleep_for(std::chrono::duration<float, std::chrono::seconds::period>(0.25));
        turn = static_cast<Space_Types>(!static_cast<bool>(turn));
    }

    return 0;
}
