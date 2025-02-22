#if !defined(POINT_T_H)
#define POINT_T_H

#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <string>		// std::string
#include <vector>
#include <tuple>
#include <cstring>      // strtok, strdup

#include <vector>
#include <string>

using index_t = long;

/* Define a numeric only template type (C++23 concepts)
 * Used to make sure the template for creating point_t are numeric types
 * e.g.; integral or floating point.
 */
template<typename T>
concept Numeric = std::integral<T> or std::floating_point<T>;

struct point_t {
	index_t x = 0;
	index_t y = 0;
	index_t z = 0;

	point_t() {}

	/* Use a template for constructor so I don't have to worry about
	 * mixing int and unsigned int, size_t, etc. on creation.
	 */
	template <Numeric Tx, Numeric Ty>
	point_t(Tx x, Ty y, Tx z = 0) :
		x(static_cast<index_t>(x)), 
		y(static_cast<index_t>(y)), 
		z(static_cast<index_t>(z)) {
	}

	point_t(const point_t &p) :
		x(static_cast<index_t>(p.x)), 
		y(static_cast<index_t>(p.y)), 
		z(static_cast<index_t>(p.z)) {
	}

	template <Numeric Tx, Numeric Ty>
	point_t(const std::pair<Tx, Ty> &p) :
		x(static_cast<index_t>(p.first)), 
		y(static_cast<index_t>(p.second)), 
		z(0) {
	}

	point_t(const std::vector<index_t> &v) {
		if (v.size() > 0) {
			this->x = v[0];
			if (v.size() > 1) {
				this->y = v[1];
				if (v.size() > 2) {
					this->z = v[2];
				}
			}
		}
	}

	bool operator<(const point_t &rhs) const {		
		// sorts by z, then y, then x 
		if (z == rhs.z) {
			if (y == rhs.y) {
				return x < rhs.x;
			}

			return y < rhs.y;
		}

		return z < rhs.z;
	}

	bool operator==(const point_t &other) const {
		return this->x == other.x
			&& this->y == other.y
			&& this->z == other.z;
	}

	bool operator!=(const point_t &other) const {
		return !(*this == other);
	}

	point_t &operator=(const point_t &other) {
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	point_t &operator+=(const point_t& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
        return *this;
    }
 
    // friends defined inside class body are inline and are hidden from non-ADL lookup
    friend point_t operator+(point_t lhs, const point_t &rhs) {
        lhs += rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
	}

	static point_t from_string(const std::string &str) {
		auto split_numbers = [](const std::string &str) {
			const std::string digits{"-0123456789"};
			std::vector<long> result;
		
			size_t start = str.find_first_of(digits, 0);
			size_t end = str.find_first_not_of(digits, start);
			while (start != std::string::npos) {
				result.push_back(std::stol(str.substr(start)));
	
				start = str.find_first_of(digits, end);
				end = str.find_first_not_of(digits, start);	
			}
	
			return result;
		};
	
		return {split_numbers(str)};
	}
};

// read points until we hit an empty line
std::vector<point_t> read_points(std::istream& is, void (*fn)(point_t &point, const std::string &line) = nullptr);

std::ostream& operator<<(std::ostream& os, const point_t &p);
std::ostream& operator<<(std::ostream& os, const std::vector<point_t> &v);
std::istream& operator>>(std::istream& is, point_t &p);

/* hash function so can be put in unordered_map or set */
template <>
struct std::hash<point_t> {
	size_t operator()(const point_t &p) const {
		return std::hash<size_t>()((((size_t)p.z & 0xFFFF) << 32)
								 | (((size_t)p.x & 0xFFFF) << 24) 
								 | (((size_t)p.y & 0xFFFF)      ));
	}
};

struct vector_t {
	point_t p = {0, 0};
	point_t dir = {0, 0};

	vector_t(const point_t &p, const point_t &dir) 
		: p(p), dir(dir) {
	}

	template <Numeric Tx, Numeric Ty>
	vector_t(Tx x, Ty y, Tx dx, Ty dy)
		: p(x, y), dir(dx, dy) {
	}

	template <Numeric Tx, Numeric Ty>
	vector_t(Tx x, Ty y, Tx z, Tx dx, Ty dy, Tx dz) :
		p(x, y, z), dir(dx, dy, dz) {
	}

	template <Numeric Tx, Numeric Ty>
	vector_t(const std::pair<Tx, Ty> &p, const std::pair<Tx, Ty> &dir) :
		p(p), dir(dir) {
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