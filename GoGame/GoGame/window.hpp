#ifndef GO_WINDOW
#define GO_WINDOW

#ifndef _VECTOR_
#include <vector>
#endif

#ifndef GO_BOARD
#include "board.hpp"
#endif

#ifndef _MEMORY_
#include <memory>
#endif

#ifndef _WINDOWS_
#include <Windows.h>
#endif

#ifndef _STRING_
#include <string>
#endif


//Rhett Thompson
class Window {
	protected:

		int screenHeight;
		int screenWidth;

	public:

		Window(int sHeight, int sWidth): screenHeight(sHeight), screenWidth(sWidth){}

		virtual std::unique_ptr<std::pair<float, float>> get_input() = 0;
		virtual void display() = 0;
		virtual void clear() = 0;

};


//Rhett Thompson
class ConsoleWindow : public Window {

	private:

		std::vector<std::vector<wchar_t>> screen;
		std::vector<wchar_t> oneD_screen;

		std::shared_ptr<Wchar_Board> board;
		std::vector<std::vector<wchar_t>> board_copy;

		HANDLE console_handle;
		DWORD dwBytesWritten = 0;

		void place_board_on_screen();
		void one_dimensionalize();
		void draw_row_and_col_labels();
		void draw_turn_info();
		std::unique_ptr<MOUSE_EVENT_RECORD> get_mouse_coord_on_click();


	public:

		ConsoleWindow(int sHeight, int sWidth, std::shared_ptr<Board> board);
		~ConsoleWindow();

		std::unique_ptr<std::pair<float, float>> get_input();

		void display();
		void clear();
};



//Rhett Thompson
class Print_Window : public Window {

	private:

		std::vector<std::vector<char>> screen;

		std::shared_ptr<Char_Board> board;
		std::vector<std::vector<char>> board_copy;

		void place_board_on_screen();
		void draw_row_and_col_labels();
		void draw_turn_info();


	public:
		

		Print_Window(int sHeight, int sWidth, std::shared_ptr<Board> board);
		~Print_Window() {};

		std::unique_ptr<std::pair<float, float>> get_input();

		void display();
		void clear();
};







#endif
