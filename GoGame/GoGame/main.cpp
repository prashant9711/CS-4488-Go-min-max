//#include "pch.h"

#include <iostream>
#include <vector>
#include <cctype>
#include <sstream>
#include <limits>
#include <thread>
#include <chrono>

#ifndef GO_WINDOW
#include "window.hpp"
#endif

#ifndef GO_BOARD
#include "board.hpp"
#endif


using namespace std;

class GoGame {
private:
    int size;
    vector<vector<char>> board;
    char currentPlayer;

public:
    GoGame(int boardSize) : size(boardSize), board(boardSize, vector<char>(boardSize, '.')), currentPlayer('W') {}

    // Temp display for board
    void displayBoard() {
        cout << "   ";
        for (int col = 0; col < size; col++) {
            cout << col << " ";
        }
        cout << endl;

        for (int row = 0; row < size; row++) {
            cout << char('A' + row) << "  ";
            for (int col = 0; col < size; col++) {
                cout << board[row][col] << " ";
            }
            cout << endl;
        }
    }

    bool placeStone(string move) {
        if (move == "quit") return false; // Quit to menu

        if (move.length() < 2) return false;

        char rowChar = toupper(move[0]); // Convert row letter to uppercase
        int row = rowChar - 'A';

        // Convert the column number safely
        int col;
        stringstream ss(move.substr(1)); // Use stringstream
        if (!(ss >> col)) return false;  // Ensure conversion is valid

        if (row < 0 || row >= size || col < 0 || col >= size || board[row][col] != '.') {
            return false;
        }

        board[row][col] = currentPlayer;
        currentPlayer = (currentPlayer == 'W') ? 'B' : 'W';  // Swap player
        return true;
    }

    // Gameplay loop
    void play() {
        string move;
        while (true) {
            displayBoard();
            cout << "Player " << currentPlayer << ", enter move (e.g., A0) or type 'quit' to return to menu: ";
            cin >> move;

            if (!placeStone(move)) {
                if (move == "quit") return; // Quit game and return to menu
                cout << "Invalid move! Try again.\n";
            }
        }
    }
};

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
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter 1 or 2.\n";
            continue;
        }

        if (choice == 2) {
            cout << "Exiting game. Goodbye!\n";
            return;
        }
        else if (choice == 1) {
            int boardSize;
            cout << "Choose board size (9, 13, 19): ";
            cin >> boardSize;

            if (cin.fail() || (boardSize != 9 && boardSize != 13 && boardSize != 19)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid board size. Defaulting to 9x9.\n";
                boardSize = 9;
            }

            GoGame game(boardSize);
            game.play();
        }
        else {
            cout << "Invalid choice! Please enter 1 or 2.\n";
        }
    }
}

int main() {
    //mainMenu();

    
    /*int screenWidth = 120;
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

    int i = 0;
    int j = 1;
    while(1){
        board->place_stone_on_board(i % stoneField_size + 1,  i / stoneField_size + 1, 0);
        board->place_stone_on_board(j % stoneField_size + 1, j / stoneField_size + 1, 1);
        //board->place_stone_on_board(i, i + 1, 1);
        window.display();
        window.clear();
        i += 2;
        j += 2;
        std::this_thread::sleep_for(std::chrono::duration<float, std::chrono::seconds::period>(0.25));
    }*/
    return 0;
}
