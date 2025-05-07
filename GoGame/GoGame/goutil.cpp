#ifndef GO_UTIL
#include "goutil.hpp"
#endif


namespace Go_Util {

	//Rhett Thompson
<<<<<<< HEAD
	//Pass in a 1 to have it lowercase the input
=======
	/**
	  * Gets keyboard input from the user
	  * @param lower: boolean for if you want the input lowercased (0 will not, 1 will)
	  */
>>>>>>> 5ee9c55 (some changes to alpha beta)
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