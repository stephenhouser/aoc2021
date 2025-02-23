#if !defined(CHARMAP_T_H)
#define CHARMAP_T_H

#include <string>		// std::string
#include <vector>		// std::vector
#include <functional>	// std::function
#include <ranges>		// std::flat_map
#include <format>
#include <fstream>
#include <iterator>

#include "point.h"

struct charmap_t {
	dimension_t size_x = 0;
	dimension_t size_y = 0;
	// size_t show_context = 0;
	std::vector<std::vector<char>> data = {};

	charmap_t() {
	}

	// empty of size_x x size_y
	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	charmap_t(Tx size_x, Ty size_y, char fill = '\0') {
		for (Ty y = 0; y < size_y; y++) {
			std::vector<char> row;
			for (Tx x = 0; x < size_x; x++) {
				row.push_back(fill);
			}

			data.push_back(row);
		}

		this->update_size();
	}

	void add_line(const std::string &line) {
		this->data.push_back({line.begin(), line.end()});
		this->update_size();
	}

	void add_line(const std::vector<char> &line) {
		this->data.push_back(line);
		this->update_size();
	}

	void add_line(const char ch = '.') {
		std::vector<char> charline(static_cast<size_t>(this->size_x), ch);
		this->data.push_back(charline);
		this->update_size();
	}

	void fill_ragged(const char filler_ch = ' ') {
		// if the map has ragged x edges (on the right)
		// fill them in with empty space to make the map rectangle

		// find max_x; max width row
		size_t max_x = 0;
		for (const auto &row : this->data) {
			max_x = std::max(max_x, row.size());
		}

		// fill with filler_ch
		for (auto &row : this->data) {
			while (row.size() < max_x) {
				row.push_back(filler_ch);
			}
		}

		// update the xsize_x of the map
		this->size_x = static_cast<dimension_t>(max_x);
	}
	
	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	bool is_valid(const Tx x, const Ty y) const {
		dimension_t native_x = static_cast<dimension_t>(x);
		dimension_t native_y = static_cast<dimension_t>(y);
		return 0 <= native_x && native_x < this->size_x 
			&& 0 <= native_y && native_y < this->size_y;
	}

	bool is_valid(const point_t &p) const {
		return is_valid(p.x, p.y);
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	char get(const Tx x, const Ty y, const char invalid = '\0') const {
		size_t size_x = static_cast<size_t>(x);
		size_t size_y = static_cast<size_t>(y);
		return this->is_valid(x, y) ? data[size_y][size_x] : invalid;
	}

	char get(const point_t &p, const char invalid = '\0') const {
		return this->get(p.x, p.y, invalid);
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	void set(const Tx x, const Ty y, const char c) {
		if (this->is_valid(x, y)) {
			size_t size_x = static_cast<size_t>(x);
			size_t size_y = static_cast<size_t>(y);
			this->data[size_y][size_x] = c;
		}
	}

	void set(const point_t &p, const char c) {
		if (this->is_valid(p)) {
			this->set(p.x, p.y, c);
		}
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	bool is_char(const Tx x, const Ty y, const char c) const {
		return this->get(x, y) == c;
	}

	bool is_char(const point_t &p, const char c) const {
		return this->get(p) == c;
	}

	template <std::convertible_to<dimension_t> Tx, std::convertible_to<dimension_t> Ty>
	bool is_not_char(const Tx x, const Ty y, const char c) const {
		return !this->is_char(x, y, c);
	}

	bool is_not_char(const point_t &p, char c) const {
		return !this->is_char(p, c);
	}

	// std::views iterator for all x,y with character
	auto all_xy() const {
        return std::views::iota(0u, data.size()) |
               std::views::transform([this](size_t y) {
                   return std::views::iota(0u, data[y].size()) |
                          std::views::transform([this, y](size_t x) {
                              return std::tuple<size_t, size_t, char>(x, y, data[y][x]);
                          });
               }) |
			   std::views::join;
    }

	std::vector<point_t> _directions{{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
	auto neighbors_of(const point_t &p) const {
		return _directions |
			std::views::filter([this, &p](const point_t &direction) {
				return this->is_valid(p+direction);
			}) |
			std::views::transform([this, &p](const point_t &direction) {
				return std::pair<point_t, char>(p+direction, this->get(p+direction));
			});
	}

	// std::views iterator for all point_t with character
 	 auto all_points() const {
        return std::views::iota(0u, data.size()) |
               std::views::transform([this](size_t y) {
                   return std::views::iota(0u, this->data[y].size()) |
                          std::views::transform([this, y](size_t x) {
                              return std::pair<point_t, char>({x, y}, this->data[y][x]);
                          });
               }) | 
			   std::views::join;
    }

	/* Iterate/Enumerate over all the points in the map
	 * spits out a pair<point_t, char> for each.
	 */
	class iterator {
		private:
			dimension_t i;
			const charmap_t &map;

		public:
			iterator(dimension_t start, const charmap_t &m) : i(start), map(m) {}
			std::pair<point_t, char> operator*() { 
				dimension_t x = i % map.size_x;
				dimension_t y = i / map.size_x;
				return {{x, y}, map.get(x, y)};
			}
			iterator &operator++() { ++i; return *this; }
			bool operator!=(const iterator &other) const { return i != other.i; }
	};

	iterator begin() const { return iterator(0, *this); }
    iterator end() const { return iterator(this->size_x * this->size_y, *this); }

	point_t find_char(const char c = '^') const {
		for (dimension_t y = 0; y < this->size_y; y++) {
			for (dimension_t x = 0; x < this->size_x; x++) {
				if (this->is_char(x, y, c)) {
					return {x, y};
				}
			}
		}

		return {0, 0};
	}

	static charmap_t from_vector(const std::vector<std::string> &lines) {
		charmap_t map;

		for (const auto& line : lines) {
			map.data.push_back({line.begin(), line.end()});
		}

		map.update_size();
		return map;
	}

	static charmap_t from_stream(std::ifstream &infile) {
		charmap_t map;

		std::string line;
		for (std::string line; std::getline(infile, line); ) {
			map.data.push_back({line.begin(), line.end()});
		}

		map.update_size();
		return map;
	}

	static charmap_t from_file(const std::string &file_name) {
		std::ifstream ifs(file_name);
		return charmap_t::from_stream(ifs);
	}

	// TODO: How to limit this type to iterable types; vector, set, etc.
	template <typename T>
	static charmap_t from_points(const T &points, const char marker = '#', const char filler = '.') {
		auto bounding_box = [](const T &points) -> std::pair<point_t, point_t> {
			point_t min_p{*(points.begin())};
			point_t max_p{*(points.begin())};
			for (const auto &p : points) {
				min_p.x = std::min(min_p.x, p.x);
				min_p.y = std::min(min_p.y, p.y);
		
				max_p.x = std::max(max_p.x, p.x+1);
				max_p.y = std::max(max_p.y, p.y+1);
			}
		
			return {min_p, max_p};
		};
		
		const auto &[min, max] = bounding_box(points);

		charmap_t map((size_t)abs(max.x - min.x), (size_t)abs(max.y - min.y), filler);

		for (const auto &point : points) {
			map.set(point, marker);
		}
		
		return map;
	}

	friend struct std::formatter<charmap_t>;

	private:
		void update_size() {
			this->size_y = static_cast<dimension_t>(this->data.size());
			if (this->size_y) {
				this->size_x = static_cast<dimension_t>(this->data[0].size());
			}
		}
};


std::ostream& operator<<(std::ostream& os, const charmap_t &map);

/* std::format not quite working right on clang 16 on macOS */
template <>
struct std::formatter<charmap_t> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const charmap_t& map, std::format_context& ctx) const {
        auto out = ctx.out();
		for (const auto &row : map.data) {
			for (const auto ch : row) {
				std::format_to(out, "{}", ch);
			}
			std::format_to(out, "\n");
			// cannot format ranges yet either.
			// std::format_to(out, "{:}\n", row);
		}

		return out;
    }
};

#endif