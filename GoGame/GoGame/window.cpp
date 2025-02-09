
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



//Rhett Thompson
ConsoleWindow::ConsoleWindow(int sHeight, int sWidth, std::shared_ptr<Board> b) :
	Window(sHeight, sWidth), 
	screen(std::vector<std::vector<wchar_t>>(sHeight, std::vector<wchar_t>(sWidth, L' '))),
	oneD_screen(std::vector<wchar_t>(sHeight * sWidth, L' ')),
	board(b)
{
	hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);

	//I wish I could get the window to resize automatically
	//MoveWindow(window_handle, x, y, width, height, redraw_window);
	//MoveWindow(hConsole, 100, 100, sWidth, sHeight, TRUE);
	//SMALL_RECT rect = { 0, 0, sWidth, sHeight };
	//SetConsoleWindowInfo(hConsole, TRUE, &rect);
	
	draw_row_and_col_labels();
}

//Rhett Thompson
void ConsoleWindow::display() {
	place_board_on_screen();
	one_dimensionalize();
	//draw_turn_info();
	get_mouse_coord_on_click(); // Change this to not be called here
	WriteConsoleOutputCharacter(hConsole, oneD_screen.data(), screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
}

//Rhett Thompson
void ConsoleWindow::clear() { return; }


//Rhett Thompson
ConsoleWindow::~ConsoleWindow() {
	CloseHandle(hConsole);
}


//Rhett Thompson
void ConsoleWindow::place_board_on_screen() {
	for (int i = 0; i < board->boardHeight; i++) {
		for (int j = 0; j < board->boardWidth; j++) {
			screen[this->board->y_pos + i][this->board->x_pos + j] = board->board[i][j];
		}
	}
}

//Rhett Thompson
void ConsoleWindow::one_dimensionalize() {
	for (int i = 0; i < screenHeight * screenWidth; i++) {
		oneD_screen[i] = screen[i / screenWidth][i % screenWidth];
	}
}


//Rhett Thompson
void ConsoleWindow::draw_row_and_col_labels() {
	//Surprisingly annoying function to write due to excessive OCD
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
void ConsoleWindow::get_mouse_coord_on_click() {
	//This works, but causes the game to pause for some reason
	HANDLE consoleInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;
	SetConsoleMode(consoleInput, mode);
	INPUT_RECORD input_record;
	DWORD events;

	ReadConsoleInput(consoleInput, &input_record, 1, &events); //I believe this waits until the input buffer has something in it before proceeding

	if (input_record.EventType == MOUSE_EVENT) {
		MOUSE_EVENT_RECORD mouse_event = input_record.Event.MouseEvent;
		if (mouse_event.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
			swprintf_s(&(oneD_screen.data())[0], 30, L"Pos: %s, %s", std::to_wstring(mouse_event.dwMousePosition.X).c_str(), std::to_wstring(mouse_event.dwMousePosition.Y).c_str());
		}
	}
}






