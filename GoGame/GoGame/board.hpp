#ifndef GO_BOARD
#define GO_BOARD

#ifndef _VECTOR_
#include <vector>
#endif

class ConsoleWindow;


enum Space_Types {
	EMPTY = -1,
	WHITE = 0,
	BLACK = 1
};



//Rhett Thompson
class Board {
	friend class ConsoleWindow;

	private:
		const int boardWidth;
		const int boardHeight;
		const int stoneField_size;
		const float board_width_between_cols;
		const float board_height_between_rows;

		int x_pos = 0;
		int y_pos = 0;
		
		std::vector<std::vector<wchar_t>> board;
		std::vector<std::vector<Space_Types>> stones;
		std::vector<std::vector<wchar_t>> white_stone;
		std::vector<std::vector<wchar_t>> black_stone;

		std::vector<std::vector<wchar_t>> build_piece(wchar_t ch);
		int calc_piece_space(float space);
		void init_board();
		void draw_stone_sprites(std::vector<std::vector<wchar_t>>& board_copy) const;

	public:

		Board(int bWidth, int bHeight, int stone_amount, int x_pos, int y_pos);
		bool place_stone_on_board(int col, int row, Space_Types player);


		std::vector<std::vector<Space_Types>>& getStones() {
			return stones;
		}
		void delete_stone(int row, int col);



};


#endif
