#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <sstream>		// std::ostringstream
#include <string>		// std::string

#include "point.h"

std::ostream& operator<<(std::ostream& os, const point_t &p) {
	std::ostringstream buffer;
	os << "(" << p.x << "," << p.y;
	if (p.z) {
		os << "," << p.z;
	}
	os << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<point_t> &v) {
	for (auto &p : v) {
		os << p << " ";
	}
	return os;
}

std::istream& operator>>(std::istream& is, point_t &p) {
	char delim;
	if (is >> p.x >> delim >> p.y) {
		if (is.get() == delim) {
			is >> p.z;
		}
	}

    return is;
}

std::ostream& operator<<(std::ostream& os, const vector_t &p) {
	std::ostringstream buffer;
	os << p.p << "," << p.dir;
	return os;
}
