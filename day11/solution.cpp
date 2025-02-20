#include <chrono>       // high resolution timer
#include <cstring>      // strtok, strdup
#include <fstream>      // ifstream (reading file)
#include <iostream>		// cout
#include <iomanip>		// setw and setprecision on output
#include <cassert>		// assert macro

#include <vector>		// collectin
#include <string>		// strings
#include <ranges>		// ranges and views
#include <algorithm>	// sort
#include <numeric>		// max, reduce, etc.
#include <unordered_set>
#include <algorithm>

#include "point.h"
#include "charmap.h"

using namespace std;

using data_t = charmap_t;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

point_t popset(unordered_set<point_t> &set) {
	auto it = set.begin();
	auto item = *it;
	set.erase(it);
	return item;
}

bool increase(const point_t &point, size_t time, charmap_t &map) {
	if (map.is_valid(point)) {
		char value = map.get(point) + (char)time;
		map.set(point, value);
		return value > '9';
	}

	return false;
}


/* Iterate map through time steps and return number of flashes */
size_t flash(charmap_t &map, size_t time) {
	unordered_set<point_t> flashed;	// points that have flashed this time
	unordered_set<point_t> pending;	// points that need to flash

	// advance all points by time units and collect pending flashes
	ranges::for_each(map.all_points(), [&map, &pending, time](const auto &p) {
		auto &[point, value] = p;
		if (increase(point, time, map)) {
			pending.emplace(point);
		}
	});

	// do the flashing
	vector<point_t> all_directions = {
		{-1, -1}, { 0, -1}, { 1, -1},
		{-1,  0},           { 1,  0},
		{-1,  1}, { 0,  1}, { 1,  1}
	};
	while (!pending.empty()) {
		const point_t &flasher = popset(pending);
		flashed.emplace(flasher);

		for (auto dir : all_directions) {
			point_t point = flasher + dir;
			if (increase(point, time, map) && !flashed.contains(point)) {
				pending.emplace(point);
			}
		}
	}

	// reset all greater than 9 to 0
	auto overflows = map.all_points() | 
		views::filter([](const auto &p) { return p.second > '9'; });

	ranges::for_each(overflows, [&map](const auto &p) {
		map.set(p.first, '0');
	});

	return flashed.size();
}

/* Part 1 */
const result_t part1(const data_t &start_map) {
	// 100 steps

	size_t flashes = 0;
	charmap_t map = start_map;
	cout << map;
	for (size_t step = 0; step < 100; step++) {
		flashes += flash(map, 1);
	}

	cout << map;
	return to_string(flashes);
}

const result_t part2([[maybe_unused]] const data_t &map) {
	return to_string(0);
}

const data_t read_data(const string &filename) {
	return charmap_t::from_file(filename);
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
