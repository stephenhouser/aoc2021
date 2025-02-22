#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <sstream>		// std::ostringstream
#include <algorithm>
#include <string.h>

#include "charmap.h"

// static const char *highlight_chars = "^>v<";
static const char *highlight_chars = "";

std::ostream& operator<<(std::ostream& os, const charmap_t &map) {
	std::ostringstream buffer;

	os << "   ";
	for (size_t x = 0; x < map.size_x; ++x) {
		os << x % 10;
	}
	os << "\n";

	os << "-+-";
	for (size_t x = 0; x < map.size_x; ++x) {
		os << "-";
	}
	os << "-+-\n";

	// size_t ymin = map.show_context < 5 ? 0 : map.show_context-5;
	// size_t ymax = std::min(map.data.size(), map.show_context+5);
	size_t ymin = 0;
	size_t ymax = map.data.size();
	for (size_t y = ymin; y < ymax; y++) {
		os << y % 10 << "| ";

		auto row = map.data[y];
		for (auto xit = row.begin(); xit != row.end(); ++xit) {
			if (strchr(highlight_chars, *xit) == NULL) {
				os << *xit;
			} else {
				os << "\033[7m" << *xit << "\033[m";
			}
		}
		os << " |" << y << "\n";
	}

	os << "-+-";
	for (size_t x = 0; x < (size_t)map.size_x; ++x) {
		os << "-";
	}
	os << "-+-\n   ";
	for (size_t x = 0; x < (size_t)map.size_x; ++x) {
		os << x % 10;
	}
	os << "\n";

	return os;
}


