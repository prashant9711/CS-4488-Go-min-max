//#include "pch.h"

#include <iostream>
#include <vector>
#include <cctype>
#include <sstream>
#include <limits>
#include <thread>
#include <chrono>
#include <set>
#include <map>
#ifndef GO_WINDOW
#include "window.hpp"
#endif

#ifndef GO_BOARD
#include "board.hpp"
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
    int size;
    //vector<vector<char>> board;
    std::shared_ptr<Board> board_class;
    //char currentPlayer;
    std::shared_ptr<Window> window;
    // Added by Prashant
    Space_Types currentPlayer;
    bool hasBot = false;
    int passCount; // To keep track of pass count of the players

    //edited by Prashant changed the first player to black
public:
    GoGame(int boardSize) : size(boardSize), currentPlayer(WHITE), passCount(0) {

        int screenWidth = 120;
        int screenHeight = 60;

        int boardWidth = ((size - 1) * ((screenWidth / (size - 1)) - 2)) + 1;
        int boardHeight = ((size - 1) * ((screenHeight / (size - 1)) - 1)) + 1;

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


    // Temp display for board
    /*void displayBoard() {
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
    }*/
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
    bool placeStone(int row, int col) { //Using 0 based indexing here

        // Added by Prashant
        // Checking for pass
        /*if (move == "pass") {
            passCount++;
            currentPlayer = (currentPlayer == 'W') ? 'B' : 'W';
            return true;
        }*/
        //passCount = 0; // Passcount = 0 if the player makes a move

        //if (move.length() < 2) return false; // Invalid input check

        //char rowChar = toupper(move[0]); // Convert row letter to uppercase
        //int row = rowChar - 'A';

        // Convert the column number safely
        //int col;
        //stringstream ss(move.substr(1));
        //if (!(ss >> col)) return false;  // Ensure conversion is valid

        //if (row < 0 || row >= size || col < 0 || col >= size || board[row][col] != '.') {
            //return false; // Catch input off board
        //}
        if (row == -100 && col == -100) { //I'm using these values to represent a pass
            passCount++;
            std::cout << "Player: " << currentPlayer << " passed\n";
            return true;
        }
        passCount = 0;
        bool result = this->board_class->place_stone_on_board(row, col, currentPlayer);
        if (!result) return result;

        //board[row][col] = currentPlayer; // Set current player stone location

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
        //board[row][col] = currentPlayer;
        bool result = this->board_class->place_stone_on_board(row, col, currentPlayer);

        // Checking for captures
        checkCaptures(row, col);

        set<pair<int, int>> visited;
        // stopping illegal moves
        if (!moveCheck(row, col, currentPlayer, visited)) {
            board[row][col] = EMPTY;
            return false;
        }

        return result;
    }


    // Gameplay loop
    // Created by Ethan
    // Modified by Rhett
    void play() {
        /*string move;
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
                cout << "Player " << currentPlayer << ", enter move (e.g., A0), 'pass' to skip, or 'quit' to exit: ";
                cin >> move;

                if (!placeStone(move)) {
                    if (move == "quit") return; // Quit game and return to menu mid game
                    cout << "Invalid move! Try again.\n";
                }
            }

            else {
                // Added by Prashant
                    if (gameMode == "2") { // If Player vs Bot
                        cout << "Bot (" << currentPlayer << ") is making a move...\n";
                        if (!botMove()) {
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
    }*/
        std::unique_ptr<std::pair<float, float>> input_coords;
        window->clear();
        while (1) {
            
            window->display();
            while (1) {
                input_coords = window->get_input();
                if (this->placeStone(static_cast<int>(input_coords->first), static_cast<int>(input_coords->second))) break;
            }
            
            window->clear();

            currentPlayer = static_cast<Space_Types>(!static_cast<bool>(currentPlayer));

            if (passCount >= 2) { //if both players pass the game ends
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
            // Draw board and start gameplay loop
            GoGame game(boardSize);
            game.play();
        }

    }

    // Created by Ethan
    int main() {
        mainMenu();


        return 0;
    }

