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

using namespace std;

/* verbosity level; 0 = nothing extra, 1 = more... Set by command line. */
int verbose = 0;

using data_t = vector<point_t>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

/* Part 1 */
const result_t part1(const data_t &data) {
	point_t submarine{0, 0};
	for (const auto &dir : data) {
		submarine += dir;
	}

	auto result = submarine.x * submarine.y;
	return to_string(result);
}

const result_t part2(const data_t &data) {
	point_t submarine{0, 0, 0};
	for (const auto &dir : data) {
		if (dir.x > 0) {		// increase x and increase depth by aim * x
			submarine.x += dir.x;
			submarine.y += submarine.z * dir.x;
		} else if (dir.y < 0) { // up x, decrease aim by x
			submarine.z += dir.y;
		} else if (dir.y > 0) {	// down x, increase aim by x
			submarine.z += dir.y;
		}
	}

	auto result = submarine.x * submarine.y;
	return to_string(result);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string direction;
	size_t distance;
	while (ifs >> direction >> distance) {
		if (direction == "forward") {
			data.push_back({distance, 0});
		} else if (direction == "down") {
			data.push_back({0, distance});
		} else if (direction == "up") {
			data.push_back({0, -distance});
		}
	}

	return data;
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
