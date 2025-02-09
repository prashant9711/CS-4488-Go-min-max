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


//Rhett Thompson
class Window {
	protected:

		int screenHeight;
		int screenWidth;

	public:

		Window(int sHeight, int sWidth): screenHeight(sHeight), screenWidth(sWidth){}

		virtual void display() = 0;
		virtual void clear() = 0;

};


//Rhett Thompson
class ConsoleWindow : Window {

	private:

		std::vector<std::vector<wchar_t>> screen;
		std::vector<wchar_t> oneD_screen;

		std::shared_ptr<Board> board;

		HANDLE hConsole;
		DWORD dwBytesWritten = 0;

		void place_board_on_screen();
		void one_dimensionalize();
		void draw_row_and_col_labels();
		void draw_turn_info();
		void get_mouse_coord_on_click();


	public:

		ConsoleWindow(int sHeight, int sWidth, std::shared_ptr<Board> board);
		~ConsoleWindow();

		void display();
		void clear();
};




#endif
