#if !defined(CHARMAP_T_H)
#define CHARMAP_T_H

#include <string>		// std::string
#include <vector>		// std::vector
#include <functional>	// std::function
#include <ranges>		// std::flat_map
#include <format>
#include <fstream>
#include <iterator>

#if !defined(POINT_T_H)
template<typename T>
concept Numeric = std::integral<T> or std::floating_point<T>;
#endif

struct charmap_t {
	size_t size_x = 0;
	size_t size_y = 0;
	// size_t show_context = 0;
	std::vector<std::vector<char>> data = {};

	charmap_t() {
	}

	static charmap_t from_file(const std::string &file_name) {
		std::ifstream ifs(file_name);
		charmap_t map(ifs);
		return map;
	}

	charmap_t(std::ifstream &infile) {
		std::string line;
		for (std::string line; std::getline(infile, line); ) {
			this->data.push_back({line.begin(), line.end()});
		}

		this->update_size();
	}

	// empty of size_x x size_y
	charmap_t(size_t size_x, size_t size_y, char fill = '\0') {
		for (size_t y = 0; y < size_y; y++) {
			std::vector<char> row;
			for (size_t x = 0; x < size_x; x++) {
				row.push_back(fill);
			}

			data.push_back(row);
		}

		this->update_size();
	}

	charmap_t(const std::vector<std::string> &lines) {
		for (const auto& line : lines) {
			this->data.push_back({line.begin(), line.end()});
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
		std::vector<char> charline(this->size_x, ch);
		this->data.push_back(charline);
		this->update_size();
	}

	void fill_lines(const char filler_ch = ' ') {
		// if the map has ragged x edges (on the right)
		// fill them in with empty space to make the map rectangle

		// find max_x, updates size_x
		for (const auto &row : this->data) {
			if (row.size() > this->size_x) {
				this->size_x = row.size();
			}
		}

		// fill with filler_ch
		for (auto &row : this->data) {
			while (row.size() < this->size_x) {
				row.push_back(filler_ch);
			}
		}
	}

	// void rotate_right() {
	// 	vector<vector<char>> rotated = {};
	// 	for (size_t x = 0; x < this->size_x; x++) {
	// 		vector<char> row;
	// 		for (size_t y = 0; y < this->size_y; y++) {
	// 			row.push_back(this->data[y][x]);
	// 		}
	// 		rotated.push_back(row);
	// 	}
	// 	data = move(rotated);
	// 	this->update_size();
	// }

	bool is_valid(const long x, const long y) const {
		return 0 <= x && x < static_cast<int>(this->size_x)
			&& 0 <= y && y < static_cast<int>(this->size_y);
	}

	bool is_valid(const int x, const int y) const {
		return 0 <= x && x < static_cast<int>(this->size_x)
			&& 0 <= y && y < static_cast<int>(this->size_y);
	}

	bool is_valid(const size_t x, const size_t y) const {
		return x < static_cast<size_t>(this->size_x)
			&& y < static_cast<size_t>(this->size_y);
	}

#if defined(POINT_T_H)
	bool is_valid(const point_t &p) const {
		return 0 <= p.x && p.x < static_cast<int>(this->size_x)
			&& 0 <= p.y && p.y < static_cast<int>(this->size_y);
	}
#endif

	template <Numeric Tx, Numeric Ty>
	char get(const Tx x, const Ty y, const char invalid = '\0') const {
		return this->is_valid(x, y) ? data[static_cast<size_t>(y)][static_cast<size_t>(x)] : invalid;
	}

	// char get(const size_t x, size_t y, const char invalid = '\0') const {
	// 	return this->is_valid(x, y) ? data[y][x] : invalid;
	// }

#if defined(POINT_T_H)
	char get(const point_t &p, const char invalid = '\0') const {
		return this->is_valid(p) ? data[static_cast<size_t>(p.y)][static_cast<size_t>(p.x)] : invalid;
	}
#endif

	template <Numeric Tx, Numeric Ty>
	void set(const Tx x, const Ty y, const char c) {
		if (this->is_valid(x, y)) {
			this->data[static_cast<size_t>(y)][static_cast<size_t>(x)] = c;
		}
	}

	// void set(const size_t x, const size_t y, const char c) {
	// 	if (this->is_valid(x, y)) {
	// 		this->data[y][x] = c;
	// 	}
	// }

#if defined(POINT_T_H)
	void set(const point_t &p, const char c) {
		if (this->is_valid(p)) {
			this->data[static_cast<size_t>(p.y)][static_cast<size_t>(p.x)] = c;
		}
	}
#endif
	template <Numeric Tx, Numeric Ty>
	bool is_char(const Tx x, const Ty y, const char c) const {
		return is_valid(x, y) && data[static_cast<size_t>(y)][static_cast<size_t>(x)] == c;
	}

	// bool is_char(const size_t x, const size_t y, const char c) const {
	// 	return is_valid(x, y) && data[y][x] == c;
	// }

#if defined(POINT_T_H)
	bool is_char(const point_t &p, const char c) const {
		return is_valid(p) && data[static_cast<size_t>(p.y)][static_cast<size_t>(p.x)] == c;
	}
#endif

	template <Numeric Tx, Numeric Ty>
	bool is_not_char(const Tx x, const Ty y, const char c) const {
		return is_valid(x, y) && !is_char(x, y, c);
	}

	// bool is_not_char(const size_t x, const size_t y, char c) const {
	// 	return is_valid(x, y) && !is_char(x, y, c);
	// }

#if defined(POINT_T_H)
	bool is_not_char(const point_t &p, char c) const {
		return is_valid(p) && !is_char(p, c);
	}
#endif

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

#if defined(POINT_T_H)
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
			size_t i;
			const charmap_t &map;

		public:
			iterator(size_t start, const charmap_t &m) : i(start), map(m) {}
			std::pair<point_t, char> operator*() { 
				size_t x = i % map.size_x;
				size_t y = i / map.size_x;
				return {{x, y}, map.get(x, y)};
			}
			iterator &operator++() { ++i; return *this; }
			bool operator!=(const iterator &other) const { return i != other.i; }
	};

	iterator begin() const { return iterator(0, *this); }
    iterator end() const { return iterator(this->size_x * this->size_y, *this); }
#endif

#if defined(POINT_T_H)
	point_t find_char(const char c = '^') const {
		for (size_t y = 0; y < this->size_y; y++) {
			for (size_t x = 0; x < this->size_x; x++) {
				if (is_char(x, y, c)) {
					return {x, y};
				}
			}
		}

		return {0, 0};
	}
#else
	std::pair<size_t, size_t> find_char(const char c = '^') const {
		for (size_t y = 0; y < this->size_y; y++) {
			for (size_t x = 0; x < this->size_x; x++) {
				if (is_char(x, y, c)) {
					return {x, y};
				}
			}
		}

		return {0, 0};
	}
#endif

#if defined(POINT_T_H)
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
#endif

	// friend struct std::formatter<charmap_t>;

	private:
		void update_size() {
			this->size_y = this->data.size();
			if (this->size_y) {
				this->size_x = this->data[0].size();
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