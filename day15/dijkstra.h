
#if !defined(DIJKSTRA_H)
#define DIJKSTRA_H

#include <vector>
#include <map>

#include "point.h"
#include "charmap.h"

// dijkstra types, code below
using dist_t = std::map<vector_t, index_t>;
using pred_t = std::map<vector_t, std::vector<vector_t>>;

// Returns, min_cost, dist[], pred[]
std::tuple<size_t, dist_t, pred_t> dijkstra(
	const charmap_t &map,
	const vector_t &start,
	const point_t &end,
	index_t (*cost_fn)(const index_t cost, 
		const vector_t &current, const vector_t &neighbor, 
		const charmap_t &map) = nullptr);

index_t dijkstra_distance(const charmap_t &map, const dist_t &dist, const point_t &p);
std::vector<point_t> dijkstra_path(const point_t &end, const pred_t &pred);
void show_dijkstra_distances(const charmap_t &map, const dist_t &dist);

#endif
