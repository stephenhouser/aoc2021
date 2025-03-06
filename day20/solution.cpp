#include <chrono>       // high resolution timer
#include <cstring>      // strtok, strdup
#include <fstream>      // ifstream (reading file)
#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <cassert>		// assert macro

#include <vector>		// collection
#include <string>		// strings
#include <ranges>		// ranges and views
#include <algorithm>	// sort
#include <numeric>		// max, reduce, etc.
#include <unordered_set>
#include <print>

#include "point.h"
#include "charmap.h"

using namespace std;

using map_t = unordered_set<point_t>;

using data_t = pair<string, map_t>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

// vector<point_t> keys(const map_t &map) {
// 	vector<point_t> ks;
// 	ks.reserve(map.size());

// 	for (const auto &[k, _]: map) {
// 		ks.emplace_back(k);
// 	}
// 	return ks;
// }

pair<point_t, point_t> bounds_of(const map_t &points) {
	point_t bounds_min{*points.begin()};
	point_t bounds_max{*points.begin()};

	for (const auto &p : points) {
		bounds_min.x = std::min(bounds_min.x, p.x);
		bounds_max.x = std::max(bounds_max.x, p.x);

		bounds_min.y = std::min(bounds_min.y, p.y);
		bounds_max.y = std::max(bounds_max.y, p.y);
	}
	return {bounds_min, bounds_max};
}

pair<point_t, point_t> outset_box(pair<point_t, point_t> bounds, dimension_t n) {
	bounds.first.x -= n;
	bounds.first.y -= n;

	bounds.second.x += n;
	bounds.second.y += n;

	return bounds;
}

vector<point_t> neighbors(const point_t &p) {
	const vector<point_t> directions = {
		{-1, -1}, { 0, -1}, { 1, -1},
		{-1,  0}, { 0,  0}, { 1,  0},
		{-1,  1}, { 0,  1}, { 1,  1}
	};

	return directions |
		views::transform([&p](const point_t &dir) {
			return p+dir;
		}) |
		ranges::to<vector<point_t>>();
}

size_t pixel_value(const point_t &p, const map_t &points) {
	size_t value = 0;
	for (const auto &n : neighbors(p)) {
		value = value << 1;
		if (points.contains(n)) {
			value |= 0x01;
		}
	}

	return value;
}

/* Part 1 */
// 5171 too high
// 5179 too high, but right for someone else?
const result_t part1(const data_t &data) {
	const auto &[instructions, points] = data;
	assert(instructions.size() == 512);

	// for (const auto &[p, v] : map) {
	// 	println("{} = {}", p, v);
	// }
	// vector<point_t> pts{points.begin(), points.end()};
	// charmap_t m = charmap_t::from_points(pts);
	// cout << m;

	map_t current = points;
	map_t next;

	int iterations = 0;
	while (iterations < 50) {
		char check_char = (iterations % 2) ? '#' : '.';

		auto [bound_min, bound_max] = outset_box(bounds_of(current), 4);
		// println("{}, {} 0 is {}", bound_min, bound_max, check_char);

		for (dimension_t y = bound_min.y; y < bound_max.y; y++) {
			for (dimension_t x = bound_min.x; x < bound_max.x; x++) {
				point_t p{x, y};
				auto pixel_index = pixel_value(p, current);
				if (iterations % 2 == 1) {
					pixel_index = ~pixel_index & 0x1ff;
				}

				auto rule = instructions[pixel_index];
				// println("{} -> {} = {}", p, pixel_index, value);
				if (rule == check_char) {
					next.emplace(p);
				}
			}
		}

		swap(next, current);
		next.clear();
		iterations++;
	}

	// vector<point_t> cpts{current.begin(), current.end()};
	// charmap_t cm = charmap_t::from_points(cpts);
	// cout << cm;
	// map_t next_map;
	// for (const auto &[p, v] : map) {
	// }

	return to_string(current.size());
}

const result_t part2([[maybe_unused]] const data_t &data) {
	return to_string(0);
}

const data_t read_data(const string &filename) {
	std::ifstream ifs(filename);

	string instructions;
	getline(ifs, instructions);
	
	string ignore;
	getline(ifs, ignore);

	charmap_t map = charmap_t::from_stream(ifs);
	map_t points;
	for (const auto &p : map.all_points()) {
		if (p.w == '#') {
			points.emplace(p);
		}
	}

	return {instructions, points};
}

template <typename T>
void print_result(T result, chrono::duration<double, milli> duration) {
	const int time_width = 10;
	const int time_precision = 4;
	const int result_width = 15;

	cout << std::setw(result_width) << std::right << result;

	cout << " ("
		 << std::setw(time_width) << std::fixed << std::right 
		 << std::setprecision(time_precision)
		 << duration.count() << "ms)";
	cout << endl;
}

int main(int argc, char *argv[]) {
	const char *input_file = argv[1];
	if (argc < 2) {
		input_file = "test.txt";
	}

    auto start_time = chrono::high_resolution_clock::now();

	auto data = read_data(input_file);

	auto parse_time = chrono::high_resolution_clock::now();
	print_result("parse", (parse_time - start_time));

	result_t p1_result = part1(data);

	auto p1_time = chrono::high_resolution_clock::now();
	print_result(p1_result, (p1_time - parse_time));

	result_t p2_result = part2(data);

	auto p2_time = chrono::high_resolution_clock::now();
	print_result(p2_result, (p2_time - p1_time));

	print_result("total", (p2_time - start_time));
}
