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

#include "point.h"
#include "charmap.h"
#include "dijkstra.h"

using namespace std;

using data_t = charmap_t;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);


/* Part 1 */
const result_t part1(const data_t &map) {

	const vector_t start({0, 0}, {0, 0});
	const point_t end(map.size_x-1, map.size_y-1);

	auto [cost, dist, pred] = dijkstra(map, start, end);
	return to_string(cost);
}

/* Return the source map multipled out dx and dy with new locations 
 * set to source value + distance. Set values from 1-9 and rotate back through.
 */
charmap_t multiply(const charmap_t &map, size_t dx, size_t dy) {
	charmap_t bigmap((size_t)map.size_x * dx, (size_t)map.size_y * dy);

	for (const auto &p : bigmap.all_points()) {
		// offset in tile, position in source
		auto src_x = p.x % map.size_x;
		auto src_y = p.y % map.size_y;
		// manhatten distance from source map
		auto distance = p.x / map.size_x + p.y / map.size_y;

		// new value is source value + distance, mod with 9
		auto value = map.get(src_x, src_y) + distance;
		value = (value > '9') ? value-9 : value;

		// set value in big map
		bigmap.set(p, value);
	}

	return bigmap;
}

const result_t part2(const data_t &map) {
	const auto bigmap = multiply(map, 5, 5);

	const vector_t start({0, 0}, {0, 0});
	const point_t end(bigmap.size_x-1, bigmap.size_y-1);

	auto [cost, dist, pred] = dijkstra(bigmap, start, end);
	return to_string(cost);
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
