#if !defined(VECTOR_T_H)
#define VECTOR_T_H

#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <string>		// std::string
#include <vector>
#include <tuple>
#include <cstring>      // strtok, strdup
#include <cassert>

#include "point.h"

struct cube_t {
	point_t p1;
	point_t p2;

	cube_t() : p1(1, 1, 1), p2(-1, -1, -1) { // empty cube
	}

	cube_t(const point_t &p1, const point_t &p2) : p1(p1), p2(p2) {
		assert(p1.x <= p2.x);
		assert(p1.y <= p2.y);
		assert(p1.z <= p2.z);
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	cube_t(Tx x1, Ty y1, Tx x2, Ty y2) : p1(x1, y1), p2(x2, y2) {
		assert(p1.x <= p2.x);
		assert(p1.y <= p2.y);
	}

	template <std::convertible_to<dimension_t> Tx, 
				std::convertible_to<dimension_t> Ty,
				std::convertible_to<dimension_t> Tz>
	cube_t(Tx x1, Ty y1, Tz z1, Tx x2, Ty y2, Tz z2) : p1(x1, y1, z1), p2(x2, y2, z2) {
		assert(p1.x <= p2.x);
		assert(p1.y <= p2.y);
		assert(p1.z <= p2.z);
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	cube_t(const std::pair<Tx, Ty> &p1, const std::pair<Tx, Ty> &p2) : p1(p1), p2(p2) {
		assert(this->p1.x <= this->p2.x);
		assert(this->p1.y <= this->p2.y);
	}

	bool operator<(const cube_t &rhs) const {
		if (p1 == rhs.p1) {
			return p2 < rhs.p2;
		}

		return p1 < rhs.p1;
	}

	bool operator==(const cube_t &other) const {
		return this->p1 == other.p1 && this->p2 == other.p2;
	}

	bool is_empty() const {
		return this->p1.x > this->p2.x
			|| this->p1.y > this->p2.y
			|| this->p1.z > this->p2.z;
	}

	bool intersects(const cube_t &other) const {
		return !(this->p2.x < other.p1.x
			  || this->p1.x > other.p2.x
			  || this->p2.y < other.p1.y
			  || this->p1.y > other.p2.y
			  || this->p2.z < other.p1.z
			  || this->p1.z > other.p2.z);
	}

	cube_t intersection(const cube_t &other) const {
		point_t new_p1(std::max(this->p1.x, other.p1.x),
					   std::max(this->p1.y, other.p1.y),
					   std::max(this->p1.z, other.p1.z));
		point_t new_p2(std::min(this->p2.x, other.p2.x),
					   std::min(this->p2.y, other.p2.y),
					   std::min(this->p2.z, other.p2.z));

		if (new_p1.x <= new_p2.x
		 && new_p1.y <= new_p2.y
		 && new_p1.z <= new_p2.z) {
			return cube_t(new_p1, new_p2);
		} else {
			return cube_t(); // empty cube
		}
	}

	dimension_t volume() const {
		if (is_empty()) {
			return 0;
		}

		return (this->p2.x - this->p1.x + 1)
			 * (this->p2.y - this->p1.y + 1)
			 * (this->p2.z - this->p1.z + 1);
	}

	// cube_t &operator=(const cube_t &other) {
	// 	this->p1 = other.p1;
	// 	this->p2 = other.p2;
	// 	return *this;
	// }

	std::vector<cube_t> subtract(const cube_t &other) const {
		std::vector<cube_t> result;

		// if they don't intersect, return this cube
		if (!this->intersects(other)) {
			result.push_back(*this);
			return result;
		}

		cube_t inter = this->intersection(other);
		assert(!inter.is_empty());

		// left
		if (this->p1.x < inter.p1.x) {
			result.emplace_back(this->p1.x, this->p1.y, this->p1.z,
								inter.p1.x - 1, this->p2.y, this->p2.z);
		}

		// right
		if (this->p2.x > inter.p2.x) {
			result.emplace_back(inter.p2.x + 1, this->p1.y, this->p1.z,
								this->p2.x, this->p2.y, this->p2.z);
		}

		// front
		if (this->p1.y < inter.p1.y) {
			result.emplace_back(inter.p1.x, this->p1.y, this->p1.z,
								inter.p2.x, inter.p1.y - 1, this->p2.z);
		}

		// back
		if (this->p2.y > inter.p2.y) {
			result.emplace_back(inter.p1.x, inter.p2.y + 1, this->p1.z,
								inter.p2.x, this->p2.y, this->p2.z);
		}

		// bottom
		if (this->p1.z < inter.p1.z) {
			result.emplace_back(inter.p1.x, inter.p1.y, this->p1.z,
								inter.p2.x, inter.p2.y, inter.p1.z - 1);
		}

		// top
		if (this->p2.z > inter.p2.z) {
			result.emplace_back(inter.p1.x, inter.p1.y, inter.p2.z + 1,
								inter.p2.x, inter.p2.y, this->p2.z);
		}

		return result;
	}
};

std::ostream& operator<<(std::ostream& os, const cube_t &cube);

/* Hash function for a vector, so that it can be in unordered set and map */
// Not valid for 3D cubes
template <>
struct std::hash<cube_t> {
	size_t operator()(const cube_t &v) const {
		return std::hash<size_t>()((((size_t)v.p1.x & 0xFFFF) << 32)   
								 | (((size_t)v.p1.y & 0xFFFF) << 24)
								 | (((size_t)v.p2.x & 0xFFFF) << 16)
								 | (((size_t)v.p2.y & 0xFFFF)      ));
	}
};
#endif