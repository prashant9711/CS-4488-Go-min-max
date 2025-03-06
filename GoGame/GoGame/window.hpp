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

		bool place_stone_for_player(Space_Types player);

		void display();
		void clear();
};


//I will probably make a new kind of window here that just prints to the console instead.
	/*char arr[] = {92, 124, 47, 0};
	int i = 0;
	while (1) {
		for (int k = 0; k < 30; k++) {
			for (int j = 0; j < 50; j++) {
				printf("%c", arr[(i+k) % 3]);
			}
			printf("\n");
		}
		std::this_thread::sleep_for(std::chrono::duration<float, std::chrono::seconds::period>(0.1));
		i++;
		system("cls");
	}*/




#endif
