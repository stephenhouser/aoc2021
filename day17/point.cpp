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

/* Read points, one per line from istream until end of file or empty line.
 * optionally, call callback function to modify point before being emplaced.
 * gets copy of the line/string used to create the point for use.
 */
std::vector<point_t> read_points(std::istream& is, void (*fn)(point_t &point, const std::string &line)) {
	std::vector<point_t> points;

	std::string line;
	while (std::getline(is, line)) {
		if (line.empty()) {
			return points;
		}

		point_t pt{point_t::from_string(line)};
		if (fn != nullptr) {
			fn(pt, line);
		}

		points.emplace_back(pt);
	}

	return points;
}
