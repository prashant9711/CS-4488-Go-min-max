#ifndef GO_UTIL
#include "goutil.hpp"
#endif


namespace Go_Util {

	//Rhett Thompson
	//Pass in a 1 to have it lowercase the input
	std::string get_keyboard_input(bool lower) {
		std::string str_input;
		char user_input[MAX_CHAR_LEN];
		fgets(user_input, MAX_CHAR_LEN, stdin);

		user_input[strcspn(user_input, "\n")] = 0;
		str_input = std::string(user_input);
		if (lower) {
			for (std::string::iterator it = str_input.begin(); it != str_input.end(); it++) {
				*it = std::tolower(*it);
			}
		}

		return str_input;
	}
}
