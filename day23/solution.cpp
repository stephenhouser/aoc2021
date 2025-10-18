#include <unistd.h>     // getopt
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
#include <map>			// map
#include <queue>		// priority_queue
#include <unordered_set> // unordered_set

#include "charmap.h"

using namespace std;


/* final result is a very long unsigned int */
using result_t = uint64_t;

/* 
 * state of the system, including cost to reach this state
 * represented as a string which can be hashed easily.
 */
struct state_t {
	std::string state;
	size_t cost;

	// needed for hasing and equality comparison
	bool operator==(const state_t &other) const {
		return state == other.state;
	}
};

/* hash function so can be put in unordered_map or set */
template <>
struct std::hash<state_t> {
	size_t operator()(const state_t &sc) const {
		return std::hash<std::string>()(sc.state);
	}
};

/* cost comparison function for priority queue containing state_t */
class compare_cost {
	public:
		bool operator()(state_t &a, state_t &b) {
			return b.cost < a.cost;
		}
};


const state_t read_data(const string &filename);
template <typename T> void print_result(T result, chrono::duration<double, milli> duration);

void show_state_compact(const state_t &state) {
	cout << state.state << "  \t" << state.cost << endl;
}

void show_state(const state_t &state) {
	string board = "#############\n#...........#\n###.#.#.#.###\n  #.#.#.#.#\n  #.#.#.#.#\n  #.#.#.#.#\n  #########";

	board[15] = state.state[0];
	board[16] = state.state[1];
	board[17] = state.state[2];
	board[18] = state.state[3];
	board[19] = state.state[4];
	board[20] = state.state[5];
	board[21] = state.state[6];
	board[22] = state.state[7];
	board[23] = state.state[8];
	board[24] = state.state[9];
	board[25] = state.state[10];

	/* rooms */
	board[31] = state.state[11];
	board[33] = state.state[12];
	board[35] = state.state[13];
	board[37] = state.state[14];

	board[45] = state.state[15];
	board[47] = state.state[16];
	board[49] = state.state[17];
	board[51] = state.state[18];

	/* part 2, bigger rooms */
	board[57] = state.state[19];
	board[59] = state.state[20];
	board[61] = state.state[21];
	board[63] = state.state[22];
	board[69] = state.state[23];
	board[71] = state.state[24];
	board[73] = state.state[25];
	board[75] = state.state[26];

	cout << board << "  \t" << state.cost << endl;
}

// Valid moves from each position, represented as a map/graph
// of position index to vector of move_t structs which include
// position index, cost to move there, and restriction (which amphipod can go there
struct move_t {
	size_t position;	// position index
	size_t cost;		// cost to move here
};


const std::map<char, std::vector<size_t>> amphipod_homes = {
	{'A', {11, 15, 19, 23}},
	{'B', {12, 16, 20, 24}},
	{'C', {13, 17, 21, 25}},
	{'D', {14, 18, 22, 26}},
};

size_t amphipod_entry(char amphipod) {
	switch (amphipod) {
		case 'A': return 2;
		case 'B': return 4;
		case 'C': return 6;
		case 'D': return 8;
		default: return 0;
	}
}

size_t amphipod_cost(char amphipod) {
	switch (amphipod) {
		case 'A': return 1;
		case 'B': return 10;
		case 'C': return 100;
		case 'D': return 1000;
		default: return 0;
	}
}

/* Find all the amphipods in the game */
vector<size_t> find_amphipods(const state_t &state) {
	vector<size_t> positions;

	for (size_t i = 0; i < state.state.size(); i++) {
		char c = state.state[i];
		if (c == 'A' || c == 'B' || c == 'C' || c == 'D') {
			positions.push_back(i);
		}
	}

	return positions;
}

/* is the position occupied in the state */
bool is_occupied(const state_t &state, size_t pos) {
	return state.state[pos] != '.';
}

/* is the position a room */
bool is_room(size_t pos) {
	return pos >= 11;
}

/* is the amphipod at pos in its home room? */
bool is_home_room(size_t pos, char amphipod) {
	if (is_room(pos)) {
		assert(amphipod_homes.contains(amphipod));

		auto rooms = amphipod_homes.at(amphipod);
		return std::find(rooms.begin(), rooms.end(), pos) != rooms.end();
	}

	return false;
}

/* does the amphipod's room contain only '.' or the amphipods that belong there? */
bool room_contains_only(const state_t &state, char amphipod) {
	assert(amphipod_homes.contains(amphipod));

	auto rooms = amphipod_homes.at(amphipod);
	return std::all_of(rooms.begin(), rooms.end(), [&](size_t pos) {
		return !is_occupied(state, pos) || state.state[pos] == amphipod || state.state[pos] == '~';
	});
}
/* compute the distance between two positions */
size_t distance(size_t from, size_t to) {
	// make sure from is less than to
	if (from > to) {
		swap(from, to);
	}

	assert(from <= 10);	// in the hallway
	assert(to >= 11);	// in a room

	size_t dist = 0;
	while (to >= 11) {
		to -= 4;
		dist++;
	}

	switch(to) {
		case 7: to = 2;	break;
		case 8: to = 4;	break;
		case 9: to = 6;	break;
		case 10: to = 8; break;
	}

	if (from > to) {
		swap(from, to);
	}
	dist += (to - from);
	return dist;
}

/* return all valid moves to move out of a home */
// 11 -> 1, 0
// 11-> 3, 5, 7, 9, 10
// 15 -> 11, 1, 0
// 15 -> 11, 3, 5, 7, 9, 10
std::map<size_t, std::pair<std::vector<size_t>, std::vector<size_t>>> out_moves = {
	{11, {{1, 0}, {3, 5, 7, 9, 10}}},  // from 11 can move left, right
	{12, {{3, 1, 0}, {5, 7, 9, 10}}},
	{13, {{5, 3, 1, 0}, {7, 9, 10}}},
	{14, {{7, 5, 3, 1, 0}, {9, 10}}},

	{15, {{11, 1, 0}, {11, 3, 5, 7, 9, 10}}},
	{16, {{12, 3, 1, 0}, {12, 5, 7, 9, 10}}},
	{17, {{13, 5, 3, 1, 0}, {13, 7, 9, 10}}},
	{18, {{14, 7, 5, 3, 1, 0}, {14, 9, 10}}},

	{19, {{15, 11, 1, 0}, {15, 11, 3, 5, 7, 9, 10}}},
	{20, {{16, 12, 3, 1, 0}, {16, 12, 5, 7, 9, 10}}},
	{21, {{17, 13, 5, 3, 1, 0}, {17, 13, 7, 9, 10}}},
	{22, {{18, 14, 7, 5, 3, 1, 0}, {18, 14, 9, 10}}},

	{23, {{19, 15, 11, 1, 0}, {19, 15, 11, 3, 5, 7, 9, 10}}},
	{24, {{20, 16, 12, 3, 1, 0}, {20, 16, 12, 5, 7, 9, 10}}},
	{25, {{21, 17, 13, 5, 3, 1, 0}, {21, 17, 13, 7, 9, 10}}},
	{26, {{22, 18, 14, 7, 5, 3, 1, 0}, {22, 18, 14, 9, 10}}},
};
std::vector<move_t> move_out(const state_t &state, size_t pos) {
	std::vector<move_t> moves;
	assert(is_room(pos));

	// go left and look
	for (auto left_pos : out_moves.at(pos).first) {
		if (is_occupied(state, left_pos)) {
			break;
		}

		if (!is_room(left_pos)) {
			moves.push_back({left_pos, distance(pos, left_pos)});
		}
	}

	// go right young man
	for (auto right_pos : out_moves.at(pos).second) {
		if (is_occupied(state, right_pos)) {
			break;
		}

		if (!is_room(right_pos)) {
			moves.push_back({right_pos, distance(pos, right_pos)});
		}
	}

	return moves;
}

/* return all valid moves to move into a home */
std::vector<move_t> move_in(const state_t &state, size_t pos) {
	std::vector<move_t> moves;

	assert(!is_room(pos));

	// does my room contain only my amphipods?
	char amphipod = state.state[pos];
	if (!room_contains_only(state, amphipod)) {
		return moves;
	}

	// can I get to my home entry?
	size_t entry = amphipod_entry(amphipod);
	if (pos < entry) {
		for (size_t p = pos + 1; p <= entry; p++) {
			if (is_occupied(state, p)) {
				return moves;
			}
		}
	} else if (pos > entry) {
		for (size_t p = entry; p < pos; p++) {
			if (is_occupied(state, p)) {
				return moves;
			}
		}
	}

	auto home_rooms = amphipod_homes.at(amphipod);
	// find deepest available home room
	size_t last_pos = 0;
	for (auto room : home_rooms) {
		if (!is_occupied(state, room)) {
			last_pos = room;
		}
	}

	if (last_pos != 0) {
		moves.push_back({last_pos, distance(pos, last_pos)});
	}

	return moves;
}

/* return all the valid moves for the amphipod at pos.
 * avoid backracking to 'from' position.
 */
std::vector<move_t> valid_moves(const state_t &state, size_t pos) {
	if (is_room(pos)) {
		return move_out(state, pos);
	}

	return move_in(state, pos);
}

/* Returns all the possible next states for the amphipod at pos */
void next_states(const state_t &state, size_t pos, [[maybe_unused]] size_t from, unordered_set<state_t> &states) {
	char amphipod = state.state[pos];

	for (const auto &move : valid_moves(state, pos)) {
		// create next state from valid move
		state_t new_state = state;
		swap(new_state.state[pos], new_state.state[move.position]);
		new_state.cost += amphipod_cost(amphipod) * move.cost;

		// if this state is new or lower cost than an existing version, add it to the set
		if (!states.contains(new_state) || states.find(new_state)->cost > new_state.cost) {
			// remove existing higher cost state
			auto existing = states.find(new_state);
			if (existing != states.end()) {
				states.erase(existing);
			}

			states.insert(new_state);
			// recursive moves, don't backtrack to where we came from
			next_states(new_state, move.position, pos, states);
		}
	}
}

/* Returns all the possible next states from the current state 
 * includes seeing if any amphipod can move.
 */
unordered_set<state_t> next_states(const state_t &state) {
	unordered_set<state_t> states;

	bool debug = false;
	if (debug) {
		// run a single state expansion for testing
		// show_state_compact(state);
		next_states(state, 13, 13, states);
		show_state_compact(state);
	} else {
		for (auto pos : find_amphipods(state)) {
			next_states(state, pos, pos, states);
		}
	}

	return states;
}

/* Dijkstra's algorithm for finding the shortest path in a weighted graph.
 * In our case the graph is all possible states and the weights are the costs
 * to move between states.
 */
result_t dijkstra(const state_t &initial_state, const state_t &final_state) {
	bool debug = false;

	std::priority_queue<state_t, std::vector<state_t>, compare_cost> Q;
	std::map<string, size_t> dist;

	Q.push(initial_state);
	dist[initial_state.state] = 0;

	size_t iterations = 0;

	while (!Q.empty()) {
		if (debug && ++iterations % 10000 == 0) {
			cout << "\33[2K\r" << iterations << " iterations, " << Q.size() << " states in queue, " << dist.size() << " total states.";
			cout << std::flush;
		}

		state_t u = Q.top();
		Q.pop();

		auto states = next_states(u);
		for (const auto &state : states) {
			if (!dist.contains(state.state) || dist[state.state] > state.cost) {
				Q.push(state);
				dist[state.state] = state.cost;
			}
		}
	}

	if (debug) {
		cout << "\33[2K\rDijkstra complete after " << iterations << " iterations, " << dist.size() << " total states." << endl;
		for (auto [state, cost] : dist) {
			if (state == final_state.state) {
				cout << endl;
				cout << state << "\t" << cost;
				cout << " Final state!";
				cout << endl;
			}
		}
	}

	// return the distance to the final state
	return dist[final_state.state];
}


/* Part 1 */
result_t part1(const state_t &data) {
	state_t final_state = {"...........ABCDABCD~~~~~~~~", 0};
	state_t initial_state = {data.state + "~~~~~~~~", 0};

	assert(distance(11, 0) == 3);
	assert(distance(15, 0) == 4);
	assert(distance(12, 10) == 7);
	assert(distance(12, 3) == 2);
	assert(distance(18, 10) == 4);
	assert(distance(13, 1) == 6);

	assert(distance(0, 11) == 3);
	assert(distance(1, 15) == 3);
	assert(distance(10, 15) == 10);
	assert(distance(5, 18) == 5);

	// show_state(initial_state);
	// show_state_compact(initial_state);

	bool debug = false;
	if (debug) {
		cout << "Initial state:" << endl;
		show_state_compact(initial_state);
	
		auto states = next_states(initial_state);
		cout << "Found " << states.size() << " next states." << endl;
		for (const auto &state : states) {
			show_state_compact(state);
		}
		return 0;
	}

	return dijkstra(initial_state, final_state);
}

result_t part2([[maybe_unused]] const state_t &data) {
	state_t final_state = {"...........ABCDABCDABCDABCD", 0};

	// unfold the extra lines for the initial state
	string unfolded_state = data.state.substr(0, 15) + "DCBADBAC" + data.state.substr(15);
	state_t initial_state = {unfolded_state, 0};

	bool debug = false;
	if (debug) {
		show_state(initial_state);
		show_state_compact(initial_state);

		show_state(final_state);
		show_state_compact(final_state);
	}

	return dijkstra(initial_state, final_state);
}

const state_t read_data(const string &filename) {
	charmap_t map = charmap_t::from_file(filename);

	string data; // = "...........";
	for (size_t i = 1; i < 12; i++) {
		data.push_back(map.get(i, 1));
	}
	data.push_back(map.get(3, 2));
	data.push_back(map.get(5, 2));
	data.push_back(map.get(7, 2));
	data.push_back(map.get(9, 2));

	data.push_back(map.get(3, 3));
	data.push_back(map.get(5, 3));
	data.push_back(map.get(7, 3));
	data.push_back(map.get(9, 3));

	state_t initial_state = {data, 0};
	return initial_state;
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
