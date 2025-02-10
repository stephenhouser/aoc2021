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

using namespace std;

using board_t = vector<vector<size_t>>;

using data_t = pair<vector<size_t>, vector<board_t>>;
using result_t = string;

const data_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);


/* Remove number from all rows (and columns) on board.
 * Return true if any row or column becomes empty as a result
 */
bool remove_number(board_t &board, size_t number) {
	bool winner = false;

	for (auto &row : board) {
		erase_if(row, [number](size_t n) { return n == number; });
		if (row.empty()) {
			winner = true;
			// don't return early as we need to remove the winning number
			// completely from the board before we compute it's score!
		}
	}

	return winner;
}

/* Return the board's score  as the sum of the (unique) remaining numbers.
 */
size_t board_score(board_t &board) {
	unordered_set<size_t> numbers;

	for (auto &row : board) {
		for (auto n : row) {
			numbers.emplace(n);
		}
	}

	return reduce(numbers.begin(), numbers.end());
}

/* Part 1 
 * Return the score of the winning board * the number drawn to make it a winner
 */
const result_t part1(const data_t &data) {
	auto [numbers, boards] = data;

	for (auto &number : numbers) {
		for (auto &board : boards) {
			if (remove_number(board, number)) {
				return to_string(board_score(board) * number);
			}
		}
	}

	return to_string(0);
}

/* Part 2
 * Return the score of the last winning board * the number drawn to make it a winner
 */
const result_t part2(const data_t &data) {
	auto [numbers, boards] = data;

	// use a vector of currently active boards
	// and a vector of the boards that will continue to be active
	// add non-winning boards to the next vector after each number
	// stop when the last remaining board is a winner
	vector<board_t> active{boards};

	for (auto &number : numbers) {
		vector<board_t> next;

		// remove number and if not a winner add to next vector
		for (auto &board : active) {
			if (!remove_number(board, number)) {
				next.emplace_back(board);
			}
		}

		// check the last active board was a winner with this number
		if (next.size() == 0 && active.size() == 1) {
			auto score = board_score(active[0]);
			return to_string(score * number);
		}

		swap(next, active);
		next.clear();
	}

	return to_string(0);
}

/* Add each "column" of a board as a "row" that can be "won" if it goes empty.
 */
void add_cols(board_t &board) {
	size_t size = board.size();
	vector<vector<size_t>> cols(size);

	for (const auto &row : board) {
		for (size_t c = 0; c < size; c++) {
			cols[c].emplace_back(row[c]);
		}
	}

	for (const auto &col : cols) {
		board.emplace_back(col);
	}
}

const data_t read_data(const string &filename) {
	vector<size_t> numbers;
	vector<board_t> boards;

	std::ifstream ifs(filename);

	string line;
	if (getline(ifs, line)) {
		numbers = split_size_t(line);
	}

	board_t board;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			vector<size_t> row = split_size_t(line);
			board.emplace_back(row);
		} else if (board.size() > 0) {
			add_cols(board); // add columns as rows
			boards.emplace_back(board);
			board.clear();
		}
	}

	boards.emplace_back(board);
	return {numbers, boards};
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
