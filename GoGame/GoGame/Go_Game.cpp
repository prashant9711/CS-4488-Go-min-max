// Go_Game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <vector>
#include <string.h>
#include <cwchar>
#include <string>
#include <array>
#include <unordered_map>



const int screenWidth = 120;	//Make sure to make your terminal window size match these values.	
const int screenHeight = 60;    //I would like this program to do it, but I can't seem to figure it out right now.

const int stoneField_size = 19; //This is always a square so only one is necessary.

const int boardWidth = ((stoneField_size-1) * ((screenWidth / (stoneField_size-1)) - 2)) + 1;
const int boardHeight = ((stoneField_size-1) * ((screenHeight / (stoneField_size-1)) - 1)) + 1;


const float board_width_between_cols = static_cast<float>(boardWidth) / static_cast<float>(stoneField_size-1);
const float board_height_between_rows = static_cast<float>(boardHeight) / static_cast<float>(stoneField_size-1);



const wchar_t WHITE = L'\u2588';
const wchar_t BLACK = L'\u2B1C';



void place_board_on_screen(std::array<std::array<wchar_t, screenWidth>, screenHeight>& screen, const std::array<std::array<wchar_t, boardWidth>, boardHeight>& board, int x_pos, int y_pos) {
	for (int i = 0; i < boardHeight; i++) {
		for (int j = 0; j < boardWidth; j++) {
			screen[y_pos + i][x_pos + j] = board[i][j];
		}
	}
}

void place_stone_on_board(
	std::array<std::array<wchar_t, boardWidth>, boardHeight>& board, 
	std::array<std::array<wchar_t, stoneField_size>, stoneField_size>& stones,
	const std::vector<std::vector<wchar_t>>& stone_sprite,
	int row, 
	int col, 
	wchar_t player)
{
	if (row > stoneField_size || col > stoneField_size) return;

	//stones[row-1][col-1] = ;
	int sprite_height = stone_sprite.size();
	int sprite_width = stone_sprite[0].size();
	for (int i = 0; i < sprite_height; i++) {
		for (int j = 0; j < sprite_width; j++) {
			/*std::cout << "putting char at: "
				<< static_cast<int>(static_cast<float>(row - 1) * board_height_between_rows) - (sprite_height >> 1) + i
				<< ", "
				<< static_cast<int>(static_cast<float>(col - 1) * board_width_between_cols) - (sprite_width >> 1) + j
				<< "\n";*/
			board[static_cast<int>(static_cast<float>(row - 1) * board_height_between_rows) - (sprite_height >> 1) + i]
				[static_cast<int>(static_cast<float>(col - 1) * board_width_between_cols) - (sprite_width >> 1) + j]
				= stone_sprite[i][j];
		}
	}
	
}

void one_dimensionalize(std::array<wchar_t, screenHeight * screenWidth>& dest, const std::array<std::array<wchar_t, screenWidth>, screenHeight>& screen) {
	for (int i = 0; i < screenHeight * screenWidth; i++) {
		dest[i] = screen[i / screenWidth][i % screenWidth]; 
	}
}

int calc_piece_space(float space) {
	std::cout << "space: " << space << "\n";

	int half_space = static_cast<int>(space) >> 1;

	return half_space - !(half_space & 0x1); //Subtract 1 if its even
}

const std::vector<std::vector<wchar_t>> build_piece(wchar_t ch) {
	int height = calc_piece_space(board_height_between_rows);
	int width = calc_piece_space(board_width_between_cols);

	std::cout << "height: " << height << "\n";
	std::cout << "width: " << width << "\n";
	std::vector<std::vector<wchar_t>> white_piece(height);
	for (size_t i = 0; i < white_piece.size(); i++) {
		white_piece[i] = std::vector<wchar_t>(width);
		for (size_t j = 0; j < white_piece[i].size(); j++) {
			white_piece[i][j] = ch;
		}
	}

	return white_piece;
}





void draw_row_and_col_labels(std::array<std::array<wchar_t, screenWidth>, screenHeight>& screen, int x_pos, int y_pos) {
	//Surprisingly annoying function to write due to excessive OCD
	int label_loc = x_pos - 1;
	int k = 0;
	for (float i = 0; i < boardHeight-1; i += board_height_between_rows, k++) {
		screen[y_pos + static_cast<int>(i)][label_loc] = static_cast<wchar_t>(static_cast<char>(65 + k));
	}
	screen[y_pos + boardHeight -1][label_loc] = static_cast<wchar_t>(static_cast<char>(65 + k));
	label_loc = y_pos - 1;
	float j = 0;
	int lesser = (9 < stoneField_size) ? 9 : stoneField_size;
	for (j = 0, k = 0; k < lesser-1; j += board_width_between_cols, k++) {
		screen[label_loc][x_pos + static_cast<int>(j)] = static_cast<wchar_t>(static_cast<char>(49 + k));
	}
	screen[label_loc][x_pos + static_cast<int>(board_width_between_cols * k) - ((board_width_between_cols * k) > (boardWidth-1))] = static_cast<wchar_t>(static_cast<char>(49 + k));
	if (stoneField_size < 10) return;
	std::wstring k_string;
	for (k += 2, j += board_width_between_cols; j < boardWidth - 1; j += board_width_between_cols, k++) {
		k_string = std::to_wstring(k);
		screen[label_loc][x_pos + static_cast<int>(j)-1] = k_string[0];
		screen[label_loc][x_pos + static_cast<int>(j)] = k_string[1];
	}
	k_string = std::to_wstring(k);
	screen[label_loc][x_pos + boardWidth-2] = k_string[0];
	screen[label_loc][x_pos + boardWidth-1] = k_string[1];
}



void init_board(std::array<std::array<wchar_t, boardWidth>, boardHeight>& board) {
	for (int i = 0; i < boardHeight; i++) { //Fill board with whitespace to start
		board[i].fill(L' ');
	}
	for (float i = 0; i < boardHeight; i+=board_height_between_rows) { //Horizontal bars inside of board
		for (int j = 0; j < boardWidth; j++) {
			board[static_cast<int>(i)][j] = L'\u2500';
		}
	}
	for (float j = 0; j < boardWidth; j += board_width_between_cols) { // Vertical bars inside of board
		for (int i = 0; i < boardHeight; i++) {
			board[i][static_cast<int>(j)] = L'\u2502';
		}
	}
	for (float i = board_height_between_rows; i < boardHeight; i += board_height_between_rows) { //Cross characters on intersections
		for (float j = board_width_between_cols; j < boardWidth; j += board_width_between_cols) {
			board[static_cast<int>(i)][static_cast<int>(j)] = L'\u253C';
		}
	}
	for (int i = 1; i < boardHeight - 1; i++) { // Vertical bars right
		board[i][boardWidth - 1] = L'\u2502';
	}
	for (int j = 1; j < boardWidth - 1; j++) { // Horizontal bars bottom
		board[boardHeight - 1][j] = L'\u2500';
	}
	for (float i = board_height_between_rows; i < boardHeight; i += board_height_between_rows) { //Right side intersections
		board[static_cast<int>(i)][boardWidth - 1] = L'\u2524';
	}
	for (float j = board_width_between_cols; j < boardWidth; j += board_width_between_cols) { //Top and bottom intersections
		board[0][static_cast<int>(j)] = L'\u252C';
		board[boardHeight - 1][static_cast<int>(j)] = L'\u2534';
	}
	board[0][0] = L'\u250C'; //Top left corner
	board[0][boardWidth - 1] = L'\u2510'; //Top right corner
	board[boardHeight - 1][0] = L'\u2514'; //Bottom left
	board[boardHeight - 1][boardWidth - 1] = L'\u2518'; //Bottom right
}




int main()
{
	std::array<std::array<wchar_t, screenWidth>, screenHeight> screen;
	for (int i = 0; i < screenHeight; i++) {
		screen[i].fill(L' ');
	}

	std::array<wchar_t, screenWidth* screenHeight> oneD_screen;
	oneD_screen.fill(L' ');

	std::array<std::array<wchar_t, boardWidth>, boardHeight> board;
	init_board(board);

	std::array<std::array<wchar_t, stoneField_size>, stoneField_size> stones;
	for (int i = 0; i < stoneField_size; i++) {
		stones[i].fill(L'\0');
	}

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	//SetConsoleOutputCP(CP_UTF8);
	DWORD dwBytesWritten = 0;

	const std::vector<std::vector<wchar_t>> white = build_piece(WHITE);

	/*for (int i = 0; i < white.size(); i++) {
		for (int j = 0; j < white[i].size(); j++) {
			std::cout << "#, ";
		}
		std::cout << "\n";
	}

	std::cout << "Board width: " << boardWidth << "\n";
	std::cout << "Board height: " << boardHeight << "\n";*/
	draw_row_and_col_labels(screen, (screenWidth - boardWidth) / 2, (screenHeight - boardHeight) / 2);
	place_stone_on_board(board, stones, white, 2, 2, WHITE);
	place_stone_on_board(board, stones, white, 3, 2, WHITE);
	/*place_stone_on_board(board, stones, white, 6, 7, WHITE);
	place_stone_on_board(board, stones, white, 3, 8, WHITE);
	place_stone_on_board(board, stones, white, 5, 5, WHITE);
	place_stone_on_board(board, stones, white, 8, 8, WHITE);
	place_stone_on_board(board, stones, white, 4, 7, WHITE);*/

    while (1) {

		
		//place_stone_on_board(board, stones, 10, 11, BLACK);
		place_board_on_screen(screen, board, (screenWidth - boardWidth)/2, (screenHeight - boardHeight)/2 );
		one_dimensionalize(oneD_screen, screen);
		//break;
        WriteConsoleOutputCharacter(hConsole, oneD_screen.data(), screenWidth * screenHeight, {0,0}, &dwBytesWritten);
    }


	CloseHandle(hConsole);
	
	return 0;
}

