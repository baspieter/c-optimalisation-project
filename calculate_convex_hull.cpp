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

vector<vec2> convex_hull(vector<vec2>& v) {
	sort(v.begin() + 1, v.end(), ccwSorter(v[0]));
	if (v.size() < 4) return v;

	vector<vec2> hull;
	auto it = v.begin();
	hull.push_back(*it++);
	hull.push_back(*it++);
	hull.push_back(*it++);

	while (it != v.end()) {
		while (ccw(*(hull.rbegin() + 1), *(hull.rbegin()), *it) >= 0 && hull.size() > 2) {
			hull.pop_back();
		}
		hull.push_back(*it++);
	}

	return hull;
}