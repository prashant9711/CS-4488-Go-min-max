#ifndef GO_BOARD
#include "board.hpp"
#endif



//Rhett Thompson
Wchar_Board::Wchar_Board(int bWidth, int bHeight, int stone_amount, int x, int y):
	Board(bWidth, bHeight, stone_amount, x, y),
	board(std::vector<std::vector<wchar_t>>(bHeight, std::vector<wchar_t>(bWidth, L' ')))  //Whitespace on initialization
{
	init_board();
	white_stone = build_piece(L'\u2588');
	black_stone = build_piece(L'\u2591');
}


//Rhett Thompson
void Wchar_Board::init_board() {
	//Fill the board with the appropriate characters

	for (float i = 0; i < boardHeight; i += board_height_between_rows) { //Horizontal bars inside of board
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


//Rhett Thompson
std::vector<std::vector<wchar_t>> Wchar_Board::build_piece(wchar_t ch) {
	//Builds a character sprite for a stone from the specified character
	int height = calc_piece_space(board_height_between_rows);
	int width = calc_piece_space(board_width_between_cols);

	std::vector<std::vector<wchar_t>> white_piece(height);
	for (size_t i = 0; i < white_piece.size(); i++) {
		white_piece[i] = std::vector<wchar_t>(width);
		for (size_t j = 0; j < white_piece[i].size(); j++) {
			white_piece[i][j] = ch;
		}
	}

	return white_piece;
}


//Rhett Thompson
int Wchar_Board::calc_piece_space(float space) {
	//Calculates the width or height of a character sprite based on the size of the board.
	int half_space = static_cast<int>(space) >> 1;
	return half_space - !(half_space & 0x1); //Subtract 1 if its even
}



//Rhett Thompson
bool Wchar_Board::place_stone_on_board(int row, int col, Space_Types player){ //This uses 0 based indexing
	//Places a stone
	if (row > stoneField_size || col > stoneField_size || row < 0 || col < 0) return false;
	if (stones[row][col] != Space_Types::EMPTY) return false;

	stones[row][col] = player;
	return true;
}

//Rhett Thompson
void Wchar_Board::draw_stone_sprites(std::vector<std::vector<wchar_t>>& board_copy) const{
	//Iterate through the stones and draw their sprites
	const std::vector<std::vector<wchar_t>>* stone_sprite;
	int sprite_height;
	int sprite_width;
	int row_calc, col_calc, r, c;

	for (size_t s = 0; s < stones.size(); s++) {
		for (size_t t = 0; t < stones[s].size(); t++) {
			if (stones[s][t] == EMPTY) continue;

			stone_sprite = static_cast<bool>(stones[s][t]) ? (&black_stone) : (&white_stone);
			sprite_height = static_cast<int>(stone_sprite->size());
			sprite_width = static_cast<int>(((*stone_sprite)[0]).size());
			for (int i = 0; i < sprite_height; i++) {
				for (int j = 0; j < sprite_width; j++) {
					row_calc = static_cast<int>(static_cast<float>(s) * board_height_between_rows) - (sprite_height >> 1) + i;
					col_calc = static_cast<int>(static_cast<float>(t) * board_width_between_cols) - (sprite_width >> 1) + j;
					r = row_calc + ((sprite_height >> 1) * (row_calc < 0)) - 2 * ((sprite_height >> 1) * (row_calc > boardHeight - 1)) - ((sprite_height >> 1) == 0 && (row_calc > boardHeight - 1));
					c = col_calc + ((sprite_width >> 1) * (col_calc < 0)) - 2 * ((sprite_width >> 1) * (col_calc > boardWidth - 1)) - ((sprite_width >> 1) == 0 && (col_calc > boardWidth - 1));
					board_copy[r][c] = (*stone_sprite)[i][j];
				}
			}
		}
	}

}





Char_Board::Char_Board(int bWidth, int bHeight, int stone_amount, int x, int y) :
	Board(bWidth, bHeight, stone_amount, x, y),
	board(std::vector<std::vector<char>>(bHeight, std::vector<char>(bWidth, ' ')))  //Whitespace on initialization
{
	init_board();
	white_stone = build_piece('#');
	black_stone = build_piece('.');
}


//Rhett Thompson
void Char_Board::init_board() {
	//Fill the board with the appropriate characters

	for (float i = 0; i < boardHeight; i += board_height_between_rows) { //Horizontal bars inside of board
		for (int j = 0; j < boardWidth; j++) {
			board[static_cast<int>(i)][j] = '-';
		}
	}
	for (float j = 0; j < boardWidth; j += board_width_between_cols) { // Vertical bars inside of board
		for (int i = 0; i < boardHeight; i++) {
			board[i][static_cast<int>(j)] = '|';
		}
	}
	if (stoneField_size < 19) {
		for (float i = board_height_between_rows; i < boardHeight; i += board_height_between_rows) { //Cross characters on intersections
			for (float j = board_width_between_cols; j < boardWidth; j += board_width_between_cols) {
				board[static_cast<int>(i)][static_cast<int>(j)] = '+';
			}
		}
	}
	for (int i = 1; i < boardHeight - 1; i++) { // Vertical bars right
		board[i][boardWidth - 1] = '|';
	}
	for (int j = 1; j < boardWidth - 1; j++) { // Horizontal bars bottom
		board[boardHeight - 1][j] = '-';
	}
}


//Rhett Thompson
std::vector<std::vector<char>> Char_Board::build_piece(char ch) {
	//Builds a character sprite for a stone from the specified character
	int height = calc_piece_space(board_height_between_rows);
	int width = calc_piece_space(board_width_between_cols);

	std::vector<std::vector<char>> white_piece(height);
	for (size_t i = 0; i < white_piece.size(); i++) {
		white_piece[i] = std::vector<char>(width);
		for (size_t j = 0; j < white_piece[i].size(); j++) {
			white_piece[i][j] = ch;
		}
	}

	return white_piece;
}


//Rhett Thompson
int Char_Board::calc_piece_space(float space) {
	//Calculates the width or height of a character sprite based on the size of the board.
	int half_space = static_cast<int>(space) >> 1;
	return half_space - !(half_space & 0x1); //Subtract 1 if its even
}



//Rhett Thompson
bool Char_Board::place_stone_on_board(int row, int col, Space_Types player) { //This uses 0 based indexing
	//Places a stone
	if (row >= stoneField_size || col >= stoneField_size || row < 0 || col < 0) return false;
	if (stones[row][col] != Space_Types::EMPTY) return false;

	stones[row][col] = player;
	return true;
}

//Rhett Thompson
void Char_Board::draw_stone_sprites(std::vector<std::vector<char>>& board_copy) const {
	//Iterate through the stones and draw their sprites
	const std::vector<std::vector<char>>* stone_sprite;
	int sprite_height;
	int sprite_width;
	int row_calc, col_calc, r, c;

	for (size_t s = 0; s < stones.size(); s++) {
		for (size_t t = 0; t < stones[s].size(); t++) {
			if (stones[s][t] == EMPTY) continue;

			stone_sprite = static_cast<bool>(stones[s][t]) ? (&black_stone) : (&white_stone);
			sprite_height = static_cast<int>(stone_sprite->size());
			sprite_width = static_cast<int>(((*stone_sprite)[0]).size());
			for (int i = 0; i < sprite_height; i++) {
				for (int j = 0; j < sprite_width; j++) {
					row_calc = static_cast<int>(static_cast<float>(s) * board_height_between_rows) - (sprite_height >> 1) + i;
					col_calc = static_cast<int>(static_cast<float>(t) * board_width_between_cols) - (sprite_width >> 1) + j;
					r = row_calc + ((sprite_height >> 1) * (row_calc < 0)) - 2 * ((sprite_height >> 1) * (row_calc > boardHeight - 1)) - ((sprite_height >> 1) == 0 && (row_calc > boardHeight - 1));
					c = col_calc + ((sprite_width >> 1) * (col_calc < 0)) - 2 * ((sprite_width >> 1) * (col_calc > boardWidth - 1)) - ((sprite_width >> 1) == 0 && (col_calc > boardWidth - 1));
					board_copy[r][c] = (*stone_sprite)[i][j];
				}
			}
		}
	}

}