#ifndef GO_UTIL
#define GO_UTIL

#ifndef _STRING_
#include <string>
#endif

//Rhett Thompson
namespace Go_Util {

	static constexpr size_t MAX_CHAR_LEN = 256;

	std::string get_keyboard_input(bool lower = 0);

}


#endif
