
#ifndef GO_WINDOW
#include "window.hpp"
#endif

#ifndef GO_BOARD
#include "board.hpp"
#endif

#ifndef GO_UTIL
#include "goutil.hpp"
#endif

#ifndef _MEMORY_
#include <memory>
#endif

#ifndef _STRING_
#include <string>
#endif

#ifndef _CMATH_
#include <cmath>
#endif

#ifndef _ASSERT_
#define _ASSERT_
#include <cassert>
#endif

#ifndef _THREAD_
#include <thread>
#endif

#ifndef _ALGORITHM_
#include <algorithm>
#endif

#ifndef _INC_STDLIB
#include <stdlib.h>
#endif

#ifndef _IOSTREAM_
#include <iostream>
#endif



//Rhett Thompson
/**
  * Initialize a console screen buffer for displaying the board.
  * @param sHeight: Height of the screen buffer.
  * @param sWidth: Width of the screen buffer.
  * @param b: shared pointer to a Board object.
  */
ConsoleWindow::ConsoleWindow(int sHeight, int sWidth, std::shared_ptr<Board> b) :
	Window(sHeight, sWidth), 
	screen(std::vector<std::vector<wchar_t>>(sHeight, std::vector<wchar_t>(sWidth, L' '))),
	oneD_screen(std::vector<wchar_t>(sHeight * sWidth, L' ')),
	board(std::static_pointer_cast<Wchar_Board>(b))
{
	board_copy = board->board;

	console_handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console_handle);
	
	draw_row_and_col_labels();
}

//Rhett Thompson
/**
  * Display the board on the screen.
  */
void ConsoleWindow::display() {

	board_copy = board->board; //Copy the blank board (I'm doing it this way, for when a stone gets deleted)
	board->draw_stone_sprites(board_copy); //Draw the stones over the copy
	place_board_on_screen(); //Display the copy
	//draw_turn_info(Space_Types::WHITE);
	one_dimensionalize();
	WriteConsoleOutputCharacter(console_handle, oneD_screen.data(), screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
}


//Rhett Thompson
/**
  * Copy the blank board to the board that will be displayed.
  */
void ConsoleWindow::clear() {
	board_copy = board->board; //Copy the blank board (I'm doing it this way, for when a stone gets deleted)
}


//Rhett Thompson
ConsoleWindow::~ConsoleWindow() {
	CloseHandle(console_handle);
}


//Rhett Thompson
/**
  * Places the board in the screen vector
  */
void ConsoleWindow::place_board_on_screen() {

	for (int i = 0; i < board->boardHeight; i++) {
		for (int j = 0; j < board->boardWidth; j++) {
			screen[this->board->y_pos + i][this->board->x_pos + j] = board_copy[i][j];
		}
	}
}

//Rhett Thompson
/**
  * //One dimensionalizes the 2d screen vector
  */
void ConsoleWindow::one_dimensionalize() {
	
	for (int i = 0; i < screenHeight * screenWidth; i++) {
		oneD_screen[i] = screen[i / screenWidth][i % screenWidth];
	}
}

//Rhett Thompson
/**
  * Prints a wstring at the specified location in the screen buffer.
  */
void ConsoleWindow::print_at_loc(int x, int y, const std::wstring& word) {
	if (x < 0 || y < 0 || x >= screenWidth || y >= screenHeight) return;

	for (int i = 0; i < word.size(); i++) {
		screen[y][x + i] = word[i];
	}
}



//Rhett Thompson
/**
  * Draws the columns and rows of the board.
  */
void ConsoleWindow::draw_row_and_col_labels() {
	
	int label_loc = (board->x_pos) - 1;
	int k = 0;
	for (float i = 0; i < (board->boardHeight) - 1; i += (board->board_height_between_rows), k++) {
		screen[(board->y_pos) + static_cast<int>(i)][label_loc] = static_cast<wchar_t>(static_cast<char>(65 + k));
	}
	screen[(board->y_pos) + (board->boardHeight) - 1][label_loc] = static_cast<wchar_t>(static_cast<char>(65 + k));
	label_loc = (board->y_pos) - 1;
	float j = 0;
	int lesser = (9 < board->stoneField_size) ? 9 : board->stoneField_size;
	for (j = 0, k = 0; k < lesser - 1; j += board->board_width_between_cols, k++) {
		screen[label_loc][(board->x_pos) + static_cast<int>(j)] = static_cast<wchar_t>(static_cast<char>(49 + k));
	}
	screen[label_loc][board->x_pos + static_cast<int>(board->board_width_between_cols * k) - ((board->board_width_between_cols * k) > (board->boardWidth - 1))] = static_cast<wchar_t>(static_cast<char>(49 + k));
	if (board->stoneField_size < 10) return;
	std::wstring k_string;
	for (k += 2, j += board->board_width_between_cols; j < board->boardWidth - 1; j += board->board_width_between_cols, k++) {
		k_string = std::to_wstring(k);
		screen[label_loc][board->x_pos + static_cast<int>(j) - 1] = k_string[0];
		screen[label_loc][board->x_pos + static_cast<int>(j)] = k_string[1];
	}
	k_string = std::to_wstring(k);
	screen[label_loc][board->x_pos + board->boardWidth - 2] = k_string[0];
	screen[label_loc][board->x_pos + board->boardWidth - 1] = k_string[1];
}


//Rhett Thompson
/**
  * Not used
  */
void ConsoleWindow::draw_turn_info(Space_Types player) {
	//Working on this to display turn in top left
	std::wstring wp = (player == Space_Types::WHITE) ? L"White" : L"Black";
	this->print_at_loc(0, 0, L"Players turn: " + wp);
}


//Rhett Thompson
/**
  * Gets the mouse coordinates in the terminal when the user clicks.
  * @return Pointer to left click mouse event.
  */
std::unique_ptr<MOUSE_EVENT_RECORD> ConsoleWindow::get_mouse_coord_on_click() {
	//Gets the mouse coordinates in the console when a left click occurs
	HANDLE consoleInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;
	SetConsoleMode(consoleInput, mode);
	INPUT_RECORD input_record;
	DWORD events;

	MOUSE_EVENT_RECORD mouse_event;
	while (1) {
		FlushConsoleInputBuffer(consoleInput); //I don't want queued mouse events so clear it every iteration
		ReadConsoleInput(consoleInput, &input_record, 1, &events); //This waits until there is an event in the input buffer.
		//It includes anything relating to input, i.e, click, movement, key presses, etc.

		if (input_record.EventType == MOUSE_EVENT && ((mouse_event = input_record.Event.MouseEvent).dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)) {
				return std::make_unique<MOUSE_EVENT_RECORD>(std::move(mouse_event)); //Only way to escape is left click
		}
	}
	return nullptr;
}

//Rhett Thompson
/**
  * Gets the mouse input from the user.
  * @return Returns the row and column on the board where the user clicked.
  */
std::unique_ptr<std::pair<float, float>> ConsoleWindow::get_input() {
	//Places a stone on the board based on the mouse coordinates
	std::unique_ptr<MOUSE_EVENT_RECORD> mouse_event = get_mouse_coord_on_click();

	int true_col = static_cast<int>(std::roundf((static_cast<float>(mouse_event->dwMousePosition.X) - static_cast<float>(board->x_pos)) / board->board_width_between_cols));
	int true_row = static_cast<int>(std::roundf((static_cast<float>(mouse_event->dwMousePosition.Y) - static_cast<float>(board->y_pos)) / board->board_height_between_rows));

	return std::make_unique<std::pair<float, float>>(true_row, true_col);
}




//Rhett Thompson
/**
  * Initializes a print window for the board.
  * @param sHeight: height of the screen
  * @param sWidth: width of the screen
  * @param b: pointer to Board class.
  */
Print_Window::Print_Window(int sHeight, int sWidth, std::shared_ptr<Board> b) :
	Window(sHeight, sWidth),
	screen(std::vector<std::vector<char>>(sHeight, std::vector<char>(sWidth, ' '))),
	board(std::static_pointer_cast<Char_Board>(b))
{
	board_copy = board->board;
	draw_row_and_col_labels();
}



//Rhett Thompson
/**
  * Places the board in the screen vector
  */
void Print_Window::place_board_on_screen() {

	for (int i = 0; i < board->boardHeight; i++) {
		for (int j = 0; j < board->boardWidth; j++) {
			screen[this->board->y_pos + i][this->board->x_pos + j] = board_copy[i][j];
		}
	}
}

//Rhett Thompson
/**
  * Draws the rows and columns on the screen.
  */
void Print_Window::draw_row_and_col_labels() {
	//Places the column and row labels next to the board
	int label_loc = (board->x_pos) - 1;
	int k = 0;
	for (float i = 0; i < (board->boardHeight) - 1; i += (board->board_height_between_rows), k++) {
		screen[(board->y_pos) + static_cast<int>(i)][label_loc] = static_cast<char>(65 + k); //65 for 'A'
	}
	screen[(board->y_pos) + (board->boardHeight) - 1][label_loc] = static_cast<char>(65 + k);
	label_loc = (board->y_pos) - 1;
	float j = 0;
	int lesser = (9 < board->stoneField_size) ? 9 : board->stoneField_size;
	for (j = 0, k = 0; k < lesser - 1; j += board->board_width_between_cols, k++) {
		screen[label_loc][(board->x_pos) + static_cast<int>(j)] = static_cast<char>(49 + k);//'49 for '1'
	}
	screen[label_loc][board->x_pos + static_cast<int>(board->board_width_between_cols * k) - ((board->board_width_between_cols * k) > (board->boardWidth - 1))] = static_cast<char>(49 + k);
	if (board->stoneField_size < 10) return;
	std::string k_string;
	for (k += 2, j += board->board_width_between_cols; j < board->boardWidth - 1; j += board->board_width_between_cols, k++) {
		k_string = std::to_string(k);
		screen[label_loc][board->x_pos + static_cast<int>(j) - 1] = k_string[0];
		screen[label_loc][board->x_pos + static_cast<int>(j)] = k_string[1];
	}
	k_string = std::to_string(k);
	screen[label_loc][board->x_pos + board->boardWidth - 2] = k_string[0];
	screen[label_loc][board->x_pos + board->boardWidth - 1] = k_string[1];
}

//Rhett Thompson
/**
  * Print the window in the terminal.
  */
void Print_Window::display() {
	board->draw_stone_sprites(board_copy); //Draw the stones over the copy
	place_board_on_screen(); //Display the copy

	std::string temp;
	for (size_t i = 0; i < screen.size(); i++) { //Print the screen
		temp = std::string(screen[i].begin(), screen[i].end());
		std::cout << temp << "\n"; 
	}
}


//Rhett Thompson
/**
  * Clear the board 
  */
void Print_Window::clear() {
	board_copy = board->board;
	//system("cls");
}





//Rhett Thompson
/**
  * Gets input from the user as keystrokes
  * @return Returns the coordinates on the board that the user entered.
  */
std::unique_ptr<std::pair<float, float>> Print_Window::get_input() {

	std::cout << "Where would you like to move: ";
	std::string input = Go_Util::get_keyboard_input(1);
	std::cout << "\n";
	std::string row = "", col = "";
	int i = 0;
	for (; i < input.size(); i++) {
		if (std::isalpha(static_cast<unsigned char>(input[i]))) row += input[i];
		else break; 
	}
	for (; i < input.size(); i++) {
		if (std::isdigit(static_cast<unsigned char>(input[i]))) col += input[i];
		else break;
	}
	if ((row == "p" || row == "pass") && col.size() < 1) return std::make_unique<std::pair<float, float>>(-100, -100); //These represent a pass
	if (row.size() > 1 || col.size() < 1) return std::make_unique<std::pair<float, float>>(-1, -1); //Invalid move
	int true_col = std::stoi(col) - 1;
	int true_row = static_cast<int>(row[0]) - 97;
	return std::make_unique<std::pair<float, float>>(true_row, true_col);
}








