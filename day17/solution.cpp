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

7,2 hits
6,3 hits
9,0 hits
17, -4 misses
*/

/* Return true if x,y is in the target area.
 * The box boundaries are considered within the target area.
 */
bool contains(const int x, const int y, const box_t &box) {
	return box.first.x <= x && x <= box.second.x
		&& box.first.y <= y && y <= box.second.y;
}

/* Return the highest y coordinate reached when launching from 0,0 with
 * velocity x,y or INT_MIN if it did not hit the target area. */
int simulate(int dx, int dy, const box_t &target) {
	int x = 0;
	int y = 0;

	// This is a quirky finite-time-step process. This just brute-forces
	// by incrementing time and testing if we have hit or gone beyond the target.
	int max_y = INT_MIN;
	while (x <= target.second.x && y >= target.first.y) {
		x += max(0, dx--);
		y += dy--;

		max_y = max(y, max_y);
		if (contains(x, y, target)) {
			// cout << "OK: " << x << "," << y << " max=" << max_y << endl;
			return max_y;
		}
	}

	return INT_MIN;
}

/* Return set of x-velocity values that could possibly hit the target area.
 * Assume x starts at 0 and is never negative.
 * Assume x-velocity will always be positive
 * The box boundaries are considered within the target area.
 */
unordered_set<int> dx_hits(const box_t &target) {
	unordered_set<int> dx_vals;

	// Test all possible dx values from (0, target.max.x) as anything larger
	// would never hit the target, and x is always > 0
	for (int dx = 0; dx <= target.second.x; dx++) {
		// xv is x velocity that decrements by one (drag) each time step
		// mini-simulate iterating moving x by xv and then decrementing xv.
		// if x ends up in the target area, then save dx as a possible solution.
		int x = 0;
		for (int xv = dx; xv >= 0; xv--) {
			x += xv;
			if (target.first.x <= x && x <= target.second.x) {
				dx_vals.emplace(dx);
				break;
			}
		}
	}

	return dx_vals;
}

/* Return set of y-velocity values that could possibly hit the target area.
 * Assume y starts at 0.
 * The box boundaries are considered within the target area.
 */
unordered_set<int> dy_hits(const box_t &target) {
	unordered_set<int> dy_vals;

	// Test all possible dy values from (+target.first.y, -target.first.y)
	// These are the outer limits of what could possibly land in the target area
	for (int dy = (int)abs(target.first.y); dy >= target.first.y; dy--) {
		// yv is y velocity that decrements by one (gravity) each time step
		// mini-simulate iterating moving y by yv and then decrementing yv.
		// if y ends up in the target area, then save dy as a possible solution.
		int y = 0;
		for (int yv = dy; yv >= target.first.y; yv--) {
			y += yv;
			if (target.first.y <= y && y <= target.second.y) {
				dy_vals.emplace(dy);
			}
		}
	}

	return dy_vals;
}

/* Part 1 */
const result_t part1(const data_t &targets) {
	int result = INT_MIN;

	// there is only one target in the sample and input
	for (const auto &target : targets) {
		// get possible dx and dy values that will hit the target
		auto dx_vals = dx_hits(target);
		auto dy_vals = dy_hits(target);

		// try all combinations, keep the max y value from all of them
		for (const int dx : dx_vals) {
			for (const int dy : dy_vals) {
				auto y = simulate(dx, dy, target);
				result = max(y, result);
			}
		}

	}

	return to_string(result);
}

// 20,-10
const result_t part2([[maybe_unused]] const data_t &targets) {
	vector<point_t> success;

	// there is only one target in the sample and input
	for (const auto &target : targets) {
		// get possible dx and dy values that will hit the target
		auto dx_vals = dx_hits(target);
		auto dy_vals = dy_hits(target);

		// add all solutions to succsess vector
		for (const int dx : dx_vals) {
			for (const int dy : dy_vals) {
				if (simulate(dx, dy, target) != INT_MIN) {
					success.push_back({dx, dy});
				}
			}
		}
	}

	return to_string(success.size());
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
