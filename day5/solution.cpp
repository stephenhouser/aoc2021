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
#include <unordered_map>

#include "point.h"
#include "split.h"

using namespace std;

struct line_t {
	point_t p1{0, 0};
	point_t p2{0, 0};

	line_t(const vector<size_t> &pts) {
		if (pts.size() > 1) {
			this->p1 = {pts[0], pts[1]};
			if (pts.size() > 3) {
				this->p2 = {pts[2], pts[3]};
			}
		}
	}
};

using data_t = vector<line_t>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

/* Is the line strictly vertical */
bool is_vertical(const line_t &line) {
	return line.p1.y == line.p2.y;
}

/* Is the line strictly horizontal */
bool is_horizontal(const line_t &line) {
	return line.p1.x == line.p2.x;
}

/* Return a vector of all the points this line crosses, including endpoints. */
vector<point_t> points_on(const line_t &line) {
	vector<point_t> points;

	int dx = line.p1.x == line.p2.x ? 0 : line.p1.x < line.p2.x ? 1 : -1;
	int dy = line.p1.y == line.p2.y ? 0 : line.p1.y < line.p2.y ? 1 : -1;

	for (auto pt = line.p1; pt != line.p2; pt += {dx, dy}) {
		points.push_back(pt);
	}
	points.push_back(line.p2);

	return points;
}

/* Part 1 */
const result_t part1(const data_t &data) {
	unordered_map<point_t, size_t> points;

	/* filter only horizontal or vertical lines */
	auto lines = data | views::filter([](const auto &line) {
		return is_horizontal(line) || is_vertical(line);
	});

	/* Create a map of all points crossed, number of crosses as the value. */
	for (const line_t &line : lines) {
		for (const auto &p : points_on(line)) {
			points[p]++;
		}
	}

	/* Sum of points that have 2 or greater */
	auto overlap = transform_reduce(points.begin(), points.end(), 0, 
			std::plus{}, 
			[](auto val) { return val.second > 1 ? 1 : 0; });

	return to_string(overlap);
}

const result_t part2(const data_t &data) {
	unordered_map<point_t, size_t> points;

	/* Create a map of all points crossed, number of crosses as the value. */
	for (const line_t &line : data) {
		for (auto p : points_on(line)) {
			points[p]++;
		}
	}

	auto overlap = transform_reduce(points.begin(), points.end(), 0, std::plus{}, 
			[](auto val) { return val.second > 1 ? 1 : 0; });

	return to_string(overlap);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			data.push_back({split_size_t(line)});
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
