#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <sstream>		// std::ostringstream
#include <string>		// std::string

#include "cube.h"

std::ostream& operator<<(std::ostream& os, const cube_t &cube) {
	std::ostringstream buffer;
	os << cube.p1 << "," << cube.p2;
	return os;
}
