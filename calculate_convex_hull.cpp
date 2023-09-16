#include "precomp.h" // include (only) this in every .cpp file

// Used to sort points in ccw order about a pivot.
struct ccwSorter {
	const vec2& pivot;

	ccwSorter(const vec2& inPivot) : pivot(inPivot) { }

	bool operator()(const vec2& a, const vec2& b) {
		return ccw(pivot, a, b) < 0;
	}
};

// The z-value of the cross product of segments 
// (a, b) and (a, c). Positive means c is ccw
// from (a, b), negative cw. Zero means its collinear.
float ccw(const vec2& a, const vec2& b, const vec2& c) {
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// The Graham scan algorithm for convex hull.
// https://en.wikipedia.org/wiki/Graham_scan
vector<vec2> GrahamScan(vector<vec2> v) {

	// Sort the rest of the points in counter-clockwise order
	// from our leftmost point.
	sort(v.begin() + 1, v.end(), ccwSorter(v[0]));

	// Add our first three points to the hull.
	vector<vec2> hull;
	auto it = v.begin();
	hull.push_back(*it++);
	hull.push_back(*it++);
	hull.push_back(*it++);

	while (it != v.end()) {
		// Pop off any points that make a convex angle with *it
		while (ccw(*(hull.rbegin() + 1), *(hull.rbegin()), *it) >= 0) {
			hull.pop_back();
		}
		hull.push_back(*it++);
	}

	return hull;
}