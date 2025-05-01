#ifndef GO_BOARD
#define GO_BOARD

#ifndef _VECTOR_
#include <vector>
#endif

class ConsoleWindow;
class PrintWindow;

//Rhett Thompson
enum Space_Types {
	EMPTY = -1,
	WHITE = 0,
	BLACK = 1
};



//Rhett Thompson
class Board {
	friend class ConsoleWindow;
	friend class Print_Window;

	protected:
		std::vector<std::vector<Space_Types>> stones;

		const int boardWidth;
		const int boardHeight;
		const int stoneField_size;
		const float board_width_between_cols;
		const float board_height_between_rows;

		int x_pos = 0;
		int y_pos = 0;
		
	public:
		virtual std::vector<std::vector<Space_Types>>& getStones() = 0;
		virtual bool place_stone_on_board(int col, int row, Space_Types player) = 0;

		Board(int bWidth, int bHeight, int stone_amount, int x, int y):
			boardWidth(bWidth),
			boardHeight(bHeight),
			stoneField_size(stone_amount),
			board_width_between_cols(static_cast<float>(boardWidth) / static_cast<float>(stoneField_size - 1)),
			board_height_between_rows(static_cast<float>(boardHeight) / static_cast<float>(stoneField_size - 1)),
			stones(std::vector<std::vector<Space_Types>>(stone_amount, std::vector<Space_Types>(stone_amount, EMPTY))), //Using -1 to represent empty space
			x_pos(x),
			y_pos(y)
		{}
};



//Rhett Thompson
class Wchar_Board: public Board {
	friend class ConsoleWindow;

	private:
		
		
		std::vector<std::vector<wchar_t>> board;
		std::vector<std::vector<wchar_t>> white_stone;
		std::vector<std::vector<wchar_t>> black_stone;

		std::vector<std::vector<wchar_t>> build_piece(wchar_t ch);
		int calc_piece_space(float space);
		void init_board();
		void draw_stone_sprites(std::vector<std::vector<wchar_t>>& board_copy) const;

	public:

		Wchar_Board(int bWidth, int bHeight, int stone_amount, int x_pos, int y_pos);
		bool place_stone_on_board(int col, int row, Space_Types player);


		std::vector<std::vector<Space_Types>>& getStones() {
			return stones;
		}
		void delete_stone(int row, int col);

};


//Rhett Thompson
class Char_Board : public Board {
	friend class Print_Window;

	private:


		std::vector<std::vector<char>> board;
		std::vector<std::vector<char>> white_stone;
		std::vector<std::vector<char>> black_stone;

		std::vector<std::vector<char>> build_piece(char ch);
		int calc_piece_space(float space);
		void init_board();
		void draw_stone_sprites(std::vector<std::vector<char>>& board_copy) const;

	public:

		Char_Board(int bWidth, int bHeight, int stone_amount, int x_pos, int y_pos);
		bool place_stone_on_board(int col, int row, Space_Types player);


		std::vector<std::vector<Space_Types>>& getStones() {
			return stones;
		}
		void delete_stone(int row, int col);

};


#endif
