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

#include "split.h"
#include "point.h"
#include "charmap.h"

using namespace std;

using map_t = unordered_set<point_t>;
using fold_t = vector<point_t>;
using data_t = pair<map_t, fold_t>;

using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

map_t fold_points(const map_t &points, const point_t &fold) {
	map_t folded;

	for (const auto &point : points) {
		if (fold.y) {
			if (point.y > fold.y) {
				folded.insert({point.x, fold.y - (point.y - fold.y)});
			} else {
				folded.insert(point);
			}
		} else {
			if (point.x > fold.x) {
				folded.insert({fold.x - (point.x - fold.x), point.y});
			} else {
				folded.insert(point);
			}
		}
	}

	return folded;
}

// template <typename T>
// std::pair<point_t, point_t> bounding_box(const T &points) {
// 	point_t min_p{*(points.begin())};
// 	point_t max_p{*(points.begin())};
// 	for (const auto &p : points) {
// 		min_p.x = min(min_p.x, p.x);
// 		min_p.y = min(min_p.y, p.y);

// 		max_p.x = max(max_p.x, p.x+1);
// 		max_p.y = max(max_p.y, p.y+1);
// 	}

// 	return {min_p, max_p};
// }

// template <typename T>
// charmap_t from_points(const T &points, const char marker = '#', const char filler = '.') {
// 	const auto &[min, max] = bounding_box(points);

// 	charmap_t map((size_t)abs(max.x - min.x), (size_t)abs(max.y - min.y), filler);

// 	for (const auto &point : points) {
// 		map.set(point, marker);
// 	}
// 	return map;
// }

/* Part 1 */
const result_t part1(const data_t &data) {
	const auto &[points, folds] = data;

	const auto one_fold = fold_points(points, folds[0]);

	return to_string(one_fold.size());
}

const result_t part2(const data_t &data) {
	const auto &[points, folds] = data;

	map_t folded = points;
	for (const auto &fold : folds) {
		folded = fold_points(folded, fold);
	}

	charmap_t map = charmap_t::from_points(folded);
	cout << map;

	return to_string(0);
}

const data_t read_data(const string &filename) {
	std::ifstream ifs(filename);

	auto points = read_points(ifs);
	map_t map {points.begin(), points.end()};

	fold_t folds = read_points(ifs, [](point_t &pt, const string &line) {
		if (line.find("y=") != string::npos) {
			swap(pt.x, pt.y);
		}
	});

	return {map, folds};
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
