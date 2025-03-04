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

using namespace std::chrono;

// #ifndef GO_WINDOW
// #include "window.hpp"
// #endif

#ifndef GO_BOARD
#include "board.hpp"
#endif

#ifndef ALPHA_BETA_PRUNING_H
#include "alpha-beta-pruning.h"
#endif


using namespace std;

// Created by Ethan
class GoGame {
private:
    // Initial variables for board and game creation
    int size;
    vector<vector<char>> board;
    //std::shared_ptr<Board> board;
    char currentPlayer;
    // Added by Prashant
	int passCount; // To keep track of pass count of the players

    //edited by Prashant changed the first player to black
public:
    GoGame(int boardSize) : size(boardSize), board(boardSize, vector<char>(boardSize, '.')), currentPlayer('B'), passCount(0) {}

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
    bool moveCheck(int row, int col, char player, set<pair<int, int>>& visited) {
        if (!onBoard(row, col) || visited.count({row, col}) || board[row][col] != player) {
            return false;
        }
        visited.insert({row, col}); //checking whether the current index has been visit previously or not

        for (const auto& neighbor : getNeighbors(row, col)) {
            int r = neighbor.first;
            int c = neighbor.second;
            if (onBoard(r, c)) {
                if (board[r][c] == '.') return true;
                if (board[r][c] == player && moveCheck(r, c, player, visited)) return true;
            }
        }
        return false;
    }

    // remove the captured stones
    // Created by Prashant
    void removeStones(int row, int col, char player) {
        board[row][col] = '.';
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
    // check for captures and remove
    void checkCaptures(int row, int col) {
        char opponent = (currentPlayer == 'W') ? 'B' : 'W';

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
    bool placeStone(string move) {
        if (move == "quit") return false; // Quit to menu


		// Added by Prashant
        // Checking for pass
        if (move == "pass") {
            passCount++;
            currentPlayer = (currentPlayer == 'W') ? 'B' : 'W';
            return true;
        }
		passCount = 0; // Passcount = 0 if the player makes a move

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

		// Added by Prashant
		checkCaptures(row, col); // Check for captures

        set<pair<int, int>> visited;
        //Check for illegal moves
        if (!moveCheck(row, col, currentPlayer, visited)) {
            board[row][col] = '.';
            return false; // Illegal suicide move
        }

        currentPlayer = (currentPlayer == 'W') ? 'B' : 'W';  // Swap player
        return true;
    }

    // Added by Prashant
    // total score= n of placed stones+captured territory
    void calculateScores() {
        // initializing scores
        int whiteScore = 0, blackScore = 0;
        map<char, int> stoneCount = { {'W', 0}, {'B', 0} };
        vector<vector<bool>> visited(size, vector<bool>(size, false));

        // Count placed stones
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == 'W') stoneCount['W']++;
                if (board[row][col] == 'B') stoneCount['B']++;
            }
        }

        // Checking for captured territory and their owners
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == '.' && !visited[row][col]) {
                    set<pair<int, int>> territory;
                    set<char> surroundingColors;
                    territoryCheck(row, col, territory, surroundingColors, visited);

                    if (surroundingColors.size() == 1) {
                        char owner = *surroundingColors.begin();
                        if (owner == 'W') whiteScore += territory.size();
                        else if (owner == 'B') blackScore += territory.size();
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
    //  recursively checking for empty regions and determining their ownership
    void territoryCheck(int row, int col, set<pair<int, int>>& territory, set<char>& surroundingColors, vector<vector<bool>>& visited) {
        if (!onBoard(row, col) || visited[row][col]) return;
        visited[row][col] = true;
        if (board[row][col] == '.') {
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
    // func to allow the bot to make random moves
    bool botMove() {
        vector<pair<int, int>> emptySpaces;
        srand(time(0));
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == '.') {
                    emptySpaces.push_back({row, col});
                }
            }
        }

        if (emptySpaces.empty()) return false; // No valid moves left (pass)

        // Randomly select a move
        int randomIndex = rand() % emptySpaces.size();
        int row = emptySpaces[randomIndex].first;
        int col = emptySpaces[randomIndex].second;

        // Place the bot's stone
        board[row][col] = currentPlayer;

        // Checking for captures
        checkCaptures(row, col);

        set<pair<int, int>> visited;
        // stopping illegal moves
        if (!moveCheck(row, col, currentPlayer, visited)) {
            board[row][col] = '.';
            return false;
        }

        currentPlayer = (currentPlayer == 'W') ? 'B' : 'W'; // Switch players
        return true;
    }

    // added by Andrija Sevaljevic
    // This function will decide best move for bot based on pruning min max algorithm
    bool alphaBetaMove() {
        vector<pair<int, int>> emptySpaces;

        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == '.') {
                    emptySpaces.push_back({ row, col });
                }
            }
        }

        if (emptySpaces.empty()) return false; // No valid moves left (pass)

        // Convert char board to int board (-1 = black, 1 = white, 0 = empty)
        vector<vector<int>> intBoard(size, vector<int>(size, 0));
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (board[i][j] == 'B') intBoard[i][j] = -1;
                else if (board[i][j] == 'W') intBoard[i][j] = 1;
            }
        }

        // Create root node
        std::shared_ptr<Node> root = std::make_shared<Node>(intBoard, size);

        // Generate possible moves
        generateNChildren(root, (currentPlayer == 'W'));

        if (root->children.empty()) {
             // No possible moves
            return false;
        }

        // Run alpha-beta pruning
        int bestValue = -10000;
        std::shared_ptr<Node> bestMove = nullptr;

        for (std::shared_ptr<Node> child : root->children) {
            auto startTime = steady_clock::now();
            int eval = alphaBeta(child, 3, -10000, 10000, false, startTime);
            if (eval > bestValue) {
                bestValue = eval;
                bestMove = child;
            }
        }

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
                board[row][col] = '.';  // Undo move
                return false;
            }

            currentPlayer = (currentPlayer == 'W') ? 'B' : 'W';  // Switch turns
        }


        freeChildren(root->children);  // Free memory
        
        // std::cout << "root shared count: " << root.use_count() << "\n";
        // std::cout << "root children count: " << root->children.size() << "\n";
        return true;
    }

    bool alphaBetaMove2() {
        vector<pair<int, int>> emptySpaces;

        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == '.') {
                    emptySpaces.push_back({ row, col });
                }
            }
        }

        if (emptySpaces.empty()) return false; // No valid moves left (pass)

        // Convert char board to int board (-1 = black, 1 = white, 0 = empty)
        vector<vector<int>> intBoard(size, vector<int>(size, 0));
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (board[i][j] == 'W') intBoard[i][j] = -1;
                else if (board[i][j] == 'B') intBoard[i][j] = 1;
            }
        }

        // Create root node
        std::shared_ptr<Node> root = std::make_shared<Node>(intBoard, size);

        // Generate possible moves
        generateNChildren(root, (currentPlayer == 'B'));

        if (root->children.empty()) {
          // No possible moves
            return false;
        }

        // Run alpha-beta pruning
        int bestValue = -10000;
        std::shared_ptr<Node> bestMove = nullptr;

        for (std::shared_ptr<Node> child : root->children) {
            auto startTime = steady_clock::now();
            int eval = alphaBeta(child, 3, -10000, 10000, false, startTime);
            if (eval > bestValue) {
                bestValue = eval;
                bestMove = child;
            }
        }

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
                board[row][col] = '.';  // Undo move
                return false;
            }

            currentPlayer = (currentPlayer == 'B') ? 'W' : 'B';  // Switch turns
        }


        freeChildren(root->children);  // Free memory
        return true;
    }

    // Gameplay loop
    // Created by Ethan
    void play() {
        string move;
        string gameMode;
        // Added by Prashant
        // Choose game mode
        cout << "Choose game mode (1 for Player vs Player, 2 for Player vs Bot): ";
        cin >> gameMode;
        //end
        while (gameMode != "1" && gameMode != "2") {
            cout << "Invalid choice! Please enter 1 for Player vs Player or 2 for Player vs Bot: ";
            cin >> gameMode;
        }
        while (true) {
            displayBoard();
            //Added by Prashant
            if (currentPlayer == 'B') { // Player's turn
                /*
                cout << "Player " << currentPlayer << ", enter move (e.g., A0), 'pass' to skip, or 'quit' to exit: ";
                cin >> move;

                if (!placeStone(move)) {
                    if (move == "quit") return; // Quit game and return to menu mid game
                    cout << "Invalid move! Try again.\n";
                }
                */
                cout << "Bot (" << currentPlayer << ") is making a move...\n";
                if (!alphaBetaMove2()) {
                    cout << "No valid moves left for the bot. Passing turn.\n";
                    passCount++;
                    currentPlayer = 'B';
                }
            }

            else {
                // Added by Prashant
                    if (gameMode == "2") { // If Player vs Bot
                        cout << "Bot (" << currentPlayer << ") is making a move...\n";
                        if (!alphaBetaMove()) {
                            cout << "No valid moves left for the bot. Passing turn.\n";
                            passCount++;
                            currentPlayer = 'B';
                        }
                    } 
                    else { // For Player vs Player 
                        cout << "Player " << currentPlayer << ", enter move (e.g., A0), 'pass' to skip: ";
                        cin >> move;

                        if (!placeStone(move)) {
                            cout << "Invalid move! Try again.\n";
                        }
                    }
                }
            if (passCount >= 2) { //if both players pass the game ends
                cout << "Both players passed. Game over!\n";
                calculateScores(); //getting the scores
                return;
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
            cout << "Choose board size (5, 7, 9, 13, 19): ";
            cin >> boardSize;

            // Build board from chosen size
            if (cin.fail() || (boardSize != 5 && boardSize != 7 && boardSize != 9 && boardSize != 13 && boardSize != 19)) {
                cin.clear();
                //cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid board size. Defaulting to 5x5.\n";
                boardSize = 5;
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
    mainMenu();

    
    // int screenWidth = 120;
    // int screenHeight = 60;
    
    // int stoneField_size = 19;
    // int boardWidth = ((stoneField_size - 1) * ((screenWidth / (stoneField_size - 1)) - 2)) + 1;
    // int boardHeight = ((stoneField_size - 1) * ((screenHeight / (stoneField_size - 1)) - 1)) + 1;
    // std::shared_ptr<Board> board = std::make_shared<Board>(
    //     boardWidth,
    //     boardHeight,
    //     stoneField_size,
    //     (screenWidth - boardWidth) / 2,
    //     (screenHeight - boardHeight) / 2
    // );
    // ConsoleWindow window(screenHeight, screenWidth, board);

    // Space_Types turn = WHITE;
    // window.display();
    // while(1){

    //     while (!window.place_stone_for_player(turn)) {} //It would be better to have the board be the only one to place stones.
    //                                                     //Problem is, it depends on mouse input.  I will figure this out later.
    //     window.display();
    //     window.clear();
        
    //     std::this_thread::sleep_for(std::chrono::duration<float, std::chrono::seconds::period>(0.25));
    //     turn = static_cast<Space_Types>(!static_cast<bool>(turn));
    // }

    return 0;
}
