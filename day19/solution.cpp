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
#include <unordered_set>
#include <print>

#include "point.h"

using namespace std;

uint64_t distance(const point_t &p1, const point_t &p2) {
	uint64_t dx = (uint64_t)abs(p1.x - p2.x);
	uint64_t dy = (uint64_t)abs(p1.y - p2.y);
	uint64_t dz = (uint64_t)abs(p1.z - p2.z);
	return dx + dy + dz;
}

struct scanner_t {
	uint64_t id;
	vector<point_t> points;
	unordered_map<uint64_t, vector<pair<point_t, point_t>>> distances = {};

	// point_t origin = {0, 0, 0};
	vector<point_t> scanners = {{0, 0, 0}};

	scanner_t(const uint64_t n, const vector<point_t> &points) : id(n), points(points) {
		set_distances();
	}

	void set_points(const vector<point_t> &points) {
		this->points = points;
		set_distances();
	}

	void clear_points() {
		this->points.clear();
		this->distances.clear();
	}

	private:
	void set_distances() {
		// create map of distance -> points
		this->distances.clear();
		for (size_t i = 0; i < points.size(); i++) {
			for (size_t j = i+1; j < points.size(); j++) {
				auto d = distance(points[i], points[j]);
				this->distances[d].push_back({points[i], points[j]});
			}
		}
	}
};

using data_t = vector<scanner_t>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

// bool match_scanner(const vec)
uint64_t common_distances(const scanner_t &s1, const scanner_t &s2) {
	uint64_t in_common = 0;
	for (const auto &[distance, count] : s1.distances) {
		if (s2.distances.contains(distance)) {
			in_common += min(count.size(), s2.distances.at(distance).size());
		}
	}

	return in_common;
}

vector<vector<point_t>> rotations = {
	{{ 1,  0,  0}, { 0,  1,  0}, { 0,  0,  1}},
	{{ 0,  0,  1}, { 0,  1,  0}, {-1,  0,  0}},
	{{-1,  0,  0}, { 0,  1,  0}, { 0,  0, -1}},
	{{ 0,  0, -1}, { 0,  1,  0}, { 1,  0,  0}},

	{{ 0, -1,  0}, { 1,  0,  0}, { 0,  0,  1}},
	{{ 0,  0,  1}, { 1,  0,  0}, { 0,  1,  0}},
	{{ 0,  1,  0}, { 1,  0,  0}, { 0,  0, -1}},
	{{ 0,  0, -1}, { 1,  0,  0}, { 0, -1,  0}},

	{{ 0,  1,  0}, {-1,  0,  0}, { 0,  0,  1}},
	{{ 0,  0,  1}, {-1,  0,  0}, { 0, -1,  0}},
	{{ 0, -1,  0}, {-1,  0,  0}, { 0,  0, -1}},
	{{ 0,  0, -1}, {-1,  0,  0}, { 0,  1,  0}},

	{{ 1,  0,  0}, { 0,  0, -1}, { 0,  1,  0}},
	{{ 0,  1,  0}, { 0,  0, -1}, {-1,  0,  0}},
	{{-1,  0,  0}, { 0,  0, -1}, { 0, -1,  0}},
	{{ 0, -1,  0}, { 0,  0, -1}, { 1,  0,  0}},

	{{ 1,  0,  0}, { 0, -1,  0}, { 0,  0, -1}},
	{{ 0,  0, -1}, { 0, -1,  0}, {-1,  0,  0}},
	{{-1,  0,  0}, { 0, -1,  0}, { 0,  0,  1}},
	{{ 0,  0,  1}, { 0, -1,  0}, { 1,  0,  0}},

	{{ 1,  0,  0}, { 0,  0,  1}, { 0, -1,  0}},
	{{ 0, -1,  0}, { 0,  0,  1}, {-1,  0,  0}},
	{{-1,  0,  0}, { 0,  0,  1}, { 0,  1,  0}},
	{{ 0,  1,  0}, { 0,  0,  1}, { 1,  0,  0}},
};

point_t rotate(const point_t &p, const size_t n) {
	point_t result;
    vector<point_t> rot = rotations[n];
    result.x = p.x * rot[0].x + p.y * rot[0].y + p.z * rot[0].z;
    result.y = p.x * rot[1].x + p.y * rot[1].y + p.z * rot[1].z;
    result.z = p.x * rot[2].x + p.y * rot[2].y + p.z * rot[2].z;
    return result;
}

void print_vp(const vector<pair<point_t, point_t>> &points) {
	for (const auto &[p1, p2] : points) {
		println("{},{}\n", p1, p2);
	}
}

point_t transform_point(const point_t &p, const point_t &xform) {
	return rotate(p, (uint64_t)xform.w) + xform;
}

vector<point_t> transform_points(const vector<point_t> &points, point_t &xform) {
	vector<point_t> trans;

	for (const auto &p : points) {
		// println("{} -> {}", p, np);
		trans.emplace_back(transform_point(p, xform));
	}

	return trans;
}

void merge_scanners(scanner_t &s1, scanner_t &s2, point_t &xform) {
	unordered_set<point_t> points{s1.points.begin(), s1.points.end()};

	auto s2_points = transform_points(s2.points, xform);
	points.insert(s2_points.begin(), s2_points.end());

	vector<point_t> v_points{points.begin(), points.end()};
	// scanner_t merged(s1.id, v_points);

	s1.points.clear();
	s1.set_points(v_points);
	s2.clear_points(); //points.clear();

	// update scanners that have been merged here
	for (const auto &p : s2.scanners) {
		s1.scanners.emplace_back(transform_point(p, xform));
	}
	// s2.origin = transform_point({0,0,0}, xform);
	// return merged;
}

size_t matching_points(const scanner_t &s1, const scanner_t &s2, point_t &xform) {
	auto p1 = s1.points;
	sort(p1.begin(), p1.end());

	auto p2 = transform_points(s2.points, xform);
	sort(p2.begin(), p2.end());

	vector<point_t> common;
	set_intersection(p1.begin(), p1.end(), p2.begin(), p2.end(), back_inserter(common));

	// println("found {} macthing points {}, {} with {}", common.size(), s1.id, s2.id, xform);
	return common.size();
}

point_t align(const scanner_t &s1, const scanner_t &s2, bool verbose = false) {
	point_t offset;
	// uint64_t confirmations = 12;

	for (const auto &[d, d1] : s1.distances) {
		if (s2.distances.contains(d)) {
			auto d2 = s2.distances.at(d);
			if (d1.size() == 1 && d2.size() == 1) {
				if (verbose) {
					print("\ts{} has {} at distance {}\n\t\t", s1.id, d1.size(), d);
					print_vp(d1);
					print("\ts{} has {} at distance {}\n\t\t", s2.id, d2.size(), d);
					print_vp(d2);
				}

				point_t s_origin = d1[0].first;
				point_t s_p1 = d1[0].first - s_origin;
				point_t s_p2 = d1[0].second - s_origin;

				point_t d_origin = d2[0].first;
				point_t d_p1 = d2[0].first - d_origin;
				point_t d_p2 = d2[0].second - d_origin;
				if (verbose) {
					cout << "\t\tXlate: " << s_p1 << ", " << s_p2 << " --> ";
					cout << d_p1 << ", " << d_p2 << endl;
				}

				for (size_t r = 0; r < 24; r++) {
					point_t d = rotate(d_p2, r);
					if (d == s_p2) {
						point_t check = s_origin - rotate(d_origin, r);
						if (verbose) {
							cout << "\t\t" << d_p1 << ", " << rotate(d_p2, r) << " -- " << r << endl;
							cout << "\t\tcheck " << check << endl;
						}

						check.w = (dimension_t)r;
						auto matches = matching_points(s1, s2, check);
						if (matches >= 12) {
							return check;
						}
					}
				}
			}
		}
	}

	// println("ERROR: Transformation Not Found");
	return {};
}

vector<pair<scanner_t &, scanner_t &>> scanner_pairs(vector<scanner_t> &scanners) {
	vector<pair<scanner_t &, scanner_t &>> pairs;

	for (uint64_t i = 0; i < scanners.size(); i++) {
		if (scanners[i].points.size() != 0) {
			for (uint64_t j = i+1; j < scanners.size(); j++) {
				if (scanners[j].points.size() != 0) {
					pairs.push_back({scanners[i], scanners[j]});
				}
			}
		}
	}

	return pairs;
}

scanner_t merge_scanners(const data_t &data) {
	auto scanners = data;
	uint64_t required_matches = (12+11+10+9+8+7+6+5+4+3+2+1);

	int active_scanners = (int)scanners.size();
	while (active_scanners > 1) {

		for (uint64_t i = 0; i < scanners.size(); i++) {
			auto &s1 = scanners[i];
			if (scanners[i].points.size() != 0) {
				for (uint64_t j = i+1; j < scanners.size(); j++) {
					auto &s2 = scanners[j];

					if (scanners[j].points.size() != 0) {
						uint64_t common = common_distances(s1, s2);
						if (common >= required_matches) {
							point_t offset = align(s1, s2, false);
							if (offset.x != 0) {
								// println("merge {} + {} : common={} xform={},r={}", s1.id, s2.id, common, offset, offset.w);
								merge_scanners(s1, s2, offset);
								assert(scanners[i].points.size() > 0);
								assert(scanners[j].points.size() == 0);
								if (--active_scanners == 1) {
									return scanners[i];
								}
							}
						}
					}
				}
			}
		}
	}

	return scanner_t(0, {});
}

/* Part 1 */
const result_t part1(const data_t &data) {
	scanner_t ocean = merge_scanners(data);

	return to_string(ocean.points.size());
}

const result_t part2(const data_t &data) {
	scanner_t ocean = merge_scanners(data);

	size_t max_distance = 0;
	auto scanners = ocean.scanners;
	for (size_t i = 0; i < scanners.size(); i++) {
		for (size_t j = i; j < scanners.size(); j++) {
			auto dist = distance(scanners[i], scanners[j]);
			max_distance = max(max_distance, dist);
		}
	}

	return to_string(max_distance);
}

const data_t read_data(const string &filename) {
	data_t data;

	std::ifstream ifs(filename);

	string line;
	uint64_t scanner_n = 0;
	vector<point_t> scanner;
	while (getline(ifs, line)) {
		if (line.starts_with("---")) {
			scanner_t scanner(scanner_n++, read_points(ifs));
			data.push_back(scanner);
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
