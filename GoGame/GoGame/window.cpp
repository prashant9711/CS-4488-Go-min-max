
#ifndef GO_WINDOW
#include "window.hpp"
#endif

#ifndef GO_BOARD
#include "board.hpp"
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

#include <iostream>



//Rhett Thompson
ConsoleWindow::ConsoleWindow(int sHeight, int sWidth, std::shared_ptr<Board> b) :
	Window(sHeight, sWidth), 
	screen(std::vector<std::vector<wchar_t>>(sHeight, std::vector<wchar_t>(sWidth, L' '))),
	oneD_screen(std::vector<wchar_t>(sHeight * sWidth, L' ')),
	board(b)
{
	console_handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console_handle);

	//I wish I could get the window to resize here
	//MoveWindow(window_handle, x, y, width, height, redraw_window);
	//MoveWindow(hConsole, 100, 100, sWidth, sHeight, TRUE);
	//SMALL_RECT rect = { 0, 0, sWidth, sHeight };
	//SetConsoleWindowInfo(hConsole, TRUE, &rect);
	
	draw_row_and_col_labels();
}

//Rhett Thompson
void ConsoleWindow::display() {
	//Writes the screen characters to the console
	board_copy = board->board; //Copy the blank board (I'm doing it this way, for when a stone gets deleted)
	board->draw_stone_sprites(board_copy); //Draw the stones over the copy
	place_board_on_screen(); //Display the copy
	one_dimensionalize();
	//draw_turn_info();
	WriteConsoleOutputCharacter(console_handle, oneD_screen.data(), screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
}

//Rhett Thompson
//This does nothing here
void ConsoleWindow::clear() { return; }


//Rhett Thompson
ConsoleWindow::~ConsoleWindow() {
	CloseHandle(console_handle);
}


//Rhett Thompson
void ConsoleWindow::place_board_on_screen() {
	//Places the board in the screen vector
	try {
		for (int i = 0; i < board->boardHeight; i++) {
			for (int j = 0; j < board->boardWidth; j++) {
				screen[this->board->y_pos + i][this->board->x_pos + j] = board_copy[i][j];
			}
		}
	}
	catch (std::exception& e) {
		CloseHandle(console_handle);
		std::cerr << e.what() << std::endl;
		exit(-1);
	}
}

//Rhett Thompson
void ConsoleWindow::one_dimensionalize() {
	//One dimensionalizes a two dimensional array
	for (int i = 0; i < screenHeight * screenWidth; i++) {
		oneD_screen[i] = screen[i / screenWidth][i % screenWidth];
	}
}


//Rhett Thompson
void ConsoleWindow::draw_row_and_col_labels() {
	//Places the column and row labels next to the board
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
void ConsoleWindow::draw_turn_info() {
	//Working on this to display turn in top left
	std::wstring active_player = L"White";
}


//Rhett Thompson
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
bool ConsoleWindow::place_stone_for_player(Space_Types player) {
	//Places a stone on the board based on the mouse coordinates
	std::unique_ptr<MOUSE_EVENT_RECORD> mouse_event = get_mouse_coord_on_click();
	if (!mouse_event) return false;

	int true_col = static_cast<int>(std::roundf((static_cast<float>(mouse_event->dwMousePosition.X) - static_cast<float>(board->x_pos)) / board->board_width_between_cols)) + 1;
	int true_row = static_cast<int>(std::roundf((static_cast<float>(mouse_event->dwMousePosition.Y) - static_cast<float>(board->y_pos)) / board->board_height_between_rows)) + 1;

	return this->board->place_stone_on_board(true_row, true_col, player);
}






