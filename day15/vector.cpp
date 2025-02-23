#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <sstream>		// std::ostringstream
#include <string>		// std::string

#include "vector.h"

std::ostream& operator<<(std::ostream& os, const vector_t &p) {
	std::ostringstream buffer;
	os << p.p << "," << p.dir;
	return os;
}
