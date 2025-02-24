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
#include <climits>

#include "point.h"
#include "split.h"

using namespace std;

// bounding box of targea area
using box_t = pair<point_t, point_t>;
using data_t = vector<box_t>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);


/*
target area: x=20..30, y=-10..-5

n(n+1) / 2 = sum
n = (-1+sqrt(1+8s)) / 2

	s=20 -> n = 6
	s=30 -> n = 7


7,2 hits
6,3 hits
9,0 hits
17, -4 misses


*/

bool contains(const int x, const int y, const box_t &box) {
	return box.first.x <= x && x <= box.second.x
		&& box.first.y <= y && y <= box.second.y;
}

/* does start dx, dy hit target area */
int simulate(int dx, int dy, const box_t &target) {
	int x = 0;
	int y = 0;

	int max_y = INT_MIN;
	while (x <= target.second.x && y >= target.first.y) {
		x += max(0, dx--);
		y += dy--;

		max_y = max(y, max_y);
		if (contains(x, y, target)) {
			return max_y;
		}
	}

	return 0;
}

/* Part 1 */
const result_t part1(const data_t &targets) {
	int result = INT_MIN;

	for (const auto &target : targets) {
		cout << target.first << ", " << target.second << endl;

		unordered_set<int> dx_vals;
		unordered_set<int> dy_vals;

		int min_dx = 100;
		int max_dx = -100;
		for (int dx = 0; dx < target.second.x; dx++) {
			int tx = 0;
			for (int x = dx; x >= 0; x--) {
				tx += x;
				if (target.first.x <= tx && tx <= target.second.x) {
					min_dx = min(min_dx, dx);
					max_dx = max(max_dx, dx);
					dx_vals.emplace(dx);
					// cout << "x hits at " << tx << " with dx=" << dx << endl; 
				}
			}
		}

		// cout << "dx " << min_dx << "," << max_dx << endl;

		int min_dy = 100;
		int max_dy = -100;
		for (int dy = (int)abs(target.first.y); dy > target.first.y; dy--) {
			int ty = 0;
			for (int y = dy; y >= target.first.y; y--) {
				ty += y;
				if (target.first.y <= ty && ty <= target.second.y) {
					min_dy = min(min_dy, dy);
					max_dy = max(max_dy, dy);
					dy_vals.emplace(dy);
					// cout << "y hits at " << ty << " with dy=" << dy << endl; 
				}
			}
		}

		// cout << "dy " << min_dy << "," << max_dy << endl;

		// auto max_y = simulate(6, 3, target);
		// cout << 6 << "," << 3 << " -> " << max_y << endl;
		for (const int dx : dx_vals) {
			for (const int dy : dy_vals) {
				auto y = simulate(dx, dy, target);
				result = max(y, result);
				cout << dx << "," << dy << " -> " << y << endl;
			}
		}

	}

	return to_string(result);
}

const result_t part2([[maybe_unused]] const data_t &targets) {
	return to_string(0);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			box_t box;
			auto nums = split_int(line);
			assert(nums.size() == 4);
			box.first.x = min(nums[0], nums[1]);
			box.second.x = max(nums[0], nums[1]);
			box.first.y = min(nums[2], nums[3]);
			box.second.y = max(nums[2], nums[3]);
			data.emplace_back(box);
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
