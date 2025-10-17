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

void show_compact_state(const state_t &state) {
	cout << state.state << "  \t" << state.cost << endl;
}

void show_state(const state_t &state) {
	string board = "#############\n#...........#\n###.#.#.#.###\n  #.#.#.#.#\n  #########";

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
	board[31] = state.state[11];
	board[33] = state.state[12];
	board[35] = state.state[13];
	board[37] = state.state[14];
	board[45] = state.state[15];
	board[47] = state.state[16];
	board[49] = state.state[17];
	board[51] = state.state[18];

	cout << board << "  \t" << state.cost << endl;
}

// Valid moves from each position, represented as a map/graph
// of position index to vector of move_t structs which include
// position index, cost to move there, and restriction (which amphipod can go there
struct move_t {
	size_t position;	// position index
	size_t cost;		// cost to move here
	char restriction;	// only this amphipod can go here, '.' means any
};

/*
Positions 2, 4, 6, and 8 are not valid stopping points in the hallway.
They don't appear in the graph but the paths that go through them have
cost multipler of 2 to account for the extra step.
	0  1  2  3  4  5  6  7  8  9 10
     	 11    12    13    14
     	 15    16    17    18
		 19    20    21    22 (part 2)
		 23    24    25    25 (part 2)
*/
const std::map<size_t, std::vector<move_t>> all_moves = {
	{0, {{1, 1, '.'}}},
	{1, {{0, 1, '.'}, {3, 2, '.'}, {11, 2, 'A'}}},
	{3, {{1, 2, '.'}, {5, 2, '.'}, {11, 2, 'A'}, {12, 2, 'B'}}},
	{5, {{3, 2, '.'}, {7, 2, '.'}, {12, 2, 'B'}, {13, 2, 'C'}}},
	{7, {{5, 2, '.'}, {9, 2, '.'}, {13, 2, 'C'}, {14, 2, 'D'}}},
	{9, {{7, 2, '.'}, {10, 1, '.'}, {14, 2, 'D'}}},
	{10, {{9, 1, '.'}}},
	{11, {{1, 2, '.'}, {3, 2, '.'}, {15, 1, 'A'}}},
	{12, {{3, 2, '.'}, {5, 2, '.'}, {16, 1, 'B'}}},
	{13, {{5, 2, '.'}, {7, 2, '.'}, {17, 1, 'C'}}},
	{14, {{7, 2, '.'}, {9, 2, '.'}, {18, 1, 'D'}}},
	{15, {{11, 1, 'A'}}},
	{16, {{12, 1, 'B'}}},
	{17, {{13, 1, 'C'}}},
	{18, {{14, 1, 'D'}}},
};

const std::map<char, std::vector<size_t>> amphipod_homes = {
	{'A', {11, 15}},
	{'B', {12, 16}},
	{'C', {13, 17}},
	{'D', {14, 18}},
};

size_t amphipod_cost(char amphipod) {
	switch (amphipod) {
		case 'A':
			return 1;
		case 'B':
			return 10;
		case 'C':
			return 100;
		case 'D':
			return 1000;
		default:
			return 0;
	}
}

/* Find all the amphipods in the game */
vector<size_t> find_amphipods(const state_t &state) {
	vector<size_t> positions;

	for (auto i : views::iota(0u, 19u)) {
		if (state.state[i] != '.') {
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
		return !is_occupied(state, pos) || state.state[pos] == amphipod;
	});
}

/* Can amphipod at `from` move to `to` based on current state.
 * This is the hard part of the problem. Gettig this right!
 */ 
bool can_move(const state_t &state, size_t from, size_t to) {
	char amphipod = state.state[from];

	// don't move into a spot that has something else in it
	if (is_occupied(state, to)) {
		// cout << "Can't move " << amphipod << " from " << from << " to " << to << ": destination occupied." << endl;
		return false;
	}

	// don't move deeper into a room that isn't our home
	if (is_room(to) && !is_home_room(to, amphipod) && to > from) {
		// cout << "Can't move " << amphipod << " from " << from << " to " << to << ": can't move deeper into someone else's home." << endl;
		return false;
	}

	// don't move deeper into our home if it contains other amphipods
	if (is_home_room(to, amphipod) && to > from && !room_contains_only(state, amphipod)) {
		// cout << "Can't move " << amphipod << " from " << from << " to " << to << ": can't move into home room if it contains other amphipods." << endl;
		return false;
	}

	// don't move out of home room if it contains only our amphipods
	if (is_home_room(from, amphipod) && to < from && room_contains_only(state, amphipod)) {
		// cout << "Can't move " << amphipod << " from " << from << " to " << to << ": can't move out of home room." << endl;
		return false;
	}

	return true;
}

/* return all the valid moves for the amphipod at pos.
 * avoid backracking to 'from' position.
 */
std::vector<move_t> valid_moves(const state_t &state, size_t pos, size_t from) {
	return std::views::filter(all_moves.at(pos), 
		[&](const move_t &move) {
			return move.position != from && can_move(state, pos, move.position);
		}
	) | std::ranges::to<std::vector<move_t>>();
}

/* Returns all the possible next states for the amphipod at pos */
void next_states(const state_t &state, size_t pos, size_t from, unordered_set<state_t> &states) {
	char amphipod = state.state[pos];

	for (const auto &move : valid_moves(state, pos, from)) {
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
		next_states(state, 12, 12, states);
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
		cout << "Dijkstra complete after " << iterations << " iterations, " << dist.size() << " total states." << endl;
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
result_t part1([[maybe_unused]] const state_t &initial_state) {
	state_t final_state = {"...........ABCDABCD", 0};

	bool debug = false;
	if (debug) {
		cout << "Initial state:" << endl;
		show_state(initial_state);
	
		auto states = next_states(initial_state);
		cout << "Found " << states.size() << " next states." << endl;
		for (const auto &state : states) {
			show_state(state);
			cout << endl;
		}
	} else {
		return dijkstra(initial_state, final_state);
	}

	return 0;
}

result_t part2([[maybe_unused]] const state_t &initial_state) {
	return 0;
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
