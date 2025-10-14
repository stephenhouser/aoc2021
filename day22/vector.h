#if !defined(VECTOR_T_H)
#define VECTOR_T_H

#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <string>		// std::string
#include <vector>
#include <tuple>
#include <cstring>      // strtok, strdup

#include "point.h"

struct vector_t {
	point_t p = {0, 0};
	point_t dir = {0, 0};

	vector_t(const point_t &p, const point_t &dir) : p(p), dir(dir) {
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	vector_t(Tx x, Ty y, Tx dx, Ty dy) : p(x, y), dir(dx, dy) {
	}

	template <std::convertible_to<dimension_t> Tx, 
				std::convertible_to<dimension_t> Ty,
				std::convertible_to<dimension_t> Tz>
	vector_t(Tx x, Ty y, Tz z, Tx dx, Ty dy, Tz dz) : p(x, y, z), dir(dx, dy, dz) {
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	vector_t(const std::pair<Tx, Ty> &p, const std::pair<Tx, Ty> &dir) : p(p), dir(dir) {
	}

	bool operator<(const vector_t &rhs) const {
		if (p == rhs.p) {
			return dir < rhs.dir;
		}

		return p < rhs.p;
	}

	bool operator==(const vector_t &other) const {
		return this->p == other.p && this->dir == other.dir;
	}
};

std::ostream& operator<<(std::ostream& os, const vector_t &p);

/* Hash function for a vector, so that it can be in unordered set and map */
template <>
struct std::hash<vector_t> {
	size_t operator()(const vector_t &v) const {
		return std::hash<size_t>()((((size_t)  v.p.x & 0xFFFF) << 32)   
								 | (((size_t)  v.p.y & 0xFFFF) << 24)
								 | (((size_t)v.dir.x & 0xFFFF) << 16)
								 | (((size_t)v.dir.y & 0xFFFF)      ));
	}
};
#endif