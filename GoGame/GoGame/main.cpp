#include "pch.h"

#include <iostream>
#include <vector>
#include <cctype>
#include <sstream>
#include <limits>

using namespace std;

// Created by Ethan
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

    // Created by Ethan
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
    // Created by Ethan
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

// Created by Ethan
int main() {
    mainMenu();
    return 0;
}
