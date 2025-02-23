
#include <vector>
#include <queue>
#include <limits.h>

#include "point.h"
#include "charmap.h"
#include "dijkstra.h"

/* *** Dijkstra *** */

// Directions we are allowed to go
static std::vector<point_t> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

// /* Returns true if we have an edge between from and to. */
// static bool is_edge(const charmap_t &map, const vector_t &from, const vector_t &to) {
// 	auto from_char = map.get(from.p);
// 	auto to_char = map.get(to.p);
// 	return to_char == 'z' || to_char >= from_char-1;
// }

// used to keep Q in cost (reverse) order. this one works :-)
class compare_cost {
	public:
		bool operator() (vector_t &a, vector_t &b) {
			return b.p.z < a.p.z;
		}
};

// The cost to go from current to neighbor
static size_t default_cost(const size_t cost, 
				[[maybe_unused]] const vector_t &current, 
				[[maybe_unused]] const vector_t &neighbor, 
				[[maybe_unused]] const charmap_t &map) {
	// return = current.p.z + 1; // ((direction == u.dir) ? 1 : 1001);
	return cost + ((size_t)map.get(neighbor.p) - (size_t)'0');
}

// Have we reached the end?
// bool is_end(const charmap_t &map, const vector_t &current) {
// 	return current.p.x == map.size_x-1 && current.p.y == map.size_y-1;
// }

// Returns, min_cost, dist[], pred[]
std::tuple<size_t, dist_t, pred_t>
dijkstra(const charmap_t &map,
		 const vector_t &start,
		 const point_t &end,
		 size_t (*cost_fn)(const size_t cost, const vector_t &current, 
							const vector_t &neighbor, const charmap_t &map)) {
	dist_t dist;
	pred_t pred;
	std::priority_queue<vector_t, std::vector<vector_t>, compare_cost> Q;

	if (cost_fn == nullptr) {
		cost_fn = default_cost;
	}

	Q.push(start);
	dist[start] = 0;

	while (!Q.empty()) {
		// u <= vertex in Q with min dist[u]
		vector_t u = Q.top();
		// remove u from Q
		Q.pop();

		size_t cost = static_cast<size_t>(u.p.z);
		u.p.z = 0; // clear so we can insert into map properly

		// test if this node is the/an end node
		if (u.p == end) {
			return {cost, dist, pred};
		}

		// for each neighbor v of u in Q
		for (auto direction : directions) {
			vector_t v(u.p+direction, direction);

			size_t neighbor_cost = cost_fn(cost, u, v, map);

			// is neighbor(v) a valid move?
			if (map.is_valid(v.p)) {
				// is cost less than existing cost
				if (dist.find(v) == dist.end() || neighbor_cost < dist[v]) {
					dist[v] = neighbor_cost;

					pred[v].clear();
					pred[v].emplace_back(u);

					v.p.z = static_cast<dimension_t>(neighbor_cost);
					Q.push(v);

				} else if (neighbor_cost == dist[v]) {
					pred[v].emplace_back(u);
				}
			}
		}
	}

	return {INT_MAX, dist, pred};
}

/* Return the distance of point p from the start using the precomputed dist[]. */
size_t dijkstra_distance(const charmap_t &map, const dist_t &dist, const point_t &p) {
	if (map.is_char(p.x, p.y, 'S')) {
		return 0;
	}

	size_t distance = INT_MAX;
	for (auto dir : directions) {
		vector_t v{p.x, p.y, dir.x, dir.y};

		auto dit = dist.find(v);
		if (dit != dist.end()) {
			size_t d = dit->second;
			if (d && d < distance) {
				distance = d;
			}
		}

	}
	return distance;
}

/* Return the path from start to end using precomputed pred. */
std::vector<point_t> dijkstra_path(const point_t &end, const pred_t &pred) {
	std::vector<point_t> path;
	std::queue<vector_t> Q;

	// look backwards in all directions
	for (auto direction : directions) {
		Q.push({end, direction});
	}

	// run backwards looking for all the tiles we hit.
	while (!Q.empty()) {
		auto vertex = Q.front();
		Q.pop();


		if (!path.size() || path.back() != vertex.p) {
			path.push_back(vertex.p);
		}

		auto pit = pred.find(vertex);
		if (pit != pred.end()) {
			std::vector<vector_t> p = pit->second;
			for (const auto &predecessor : p) {
				Q.push(predecessor);
			}
		}
	}

	return path;
}

void show_dijkstra_distances(const charmap_t &map, const dist_t &dist) {
	int x_width = 4;

	std::cout << "   ";
	for (dimension_t x = 0; x < map.size_x; ++x) {
		std::cout << std::setw(x_width) << x;
	}
	std::cout << "\n";

	std::cout << "-+-";
	for (dimension_t x = 0; x < map.size_x; ++x) {
		std::cout << "---";
	}
	std::cout << "-+-\n";


	size_t y = 0;
	for (auto yit = map.data.begin(); yit != map.data.end(); ++yit) {
		std::cout << y % 10 << "| ";

		size_t x = 0;
		for (auto xit = (*yit).begin(); xit != (*yit).end(); ++xit) {
			size_t distance = dijkstra_distance(map, dist, {x, y});
			if (distance < INT_MAX) {
				std::cout << std::setw(x_width) << dijkstra_distance(map, dist, {x, y});
			} else {
				std::cout << std::setw(x_width) << " ";
			}

			x++;
		}
		std::cout << " |" << std::setw(x_width) << y << "\n";
		++y;
	}

	std::cout << "-+-";
	for (dimension_t x = 0; x < map.size_x; ++x) {
		std::cout << "---";
	}
	std::cout << "-+-\n   ";
	for (dimension_t x = 0; x < map.size_x; ++x) {
		std::cout << std::setw(x_width) << x;
	}
	std::cout << "\n";
}
