#include "precomp.h" // include (only) this in every .cpp file

// Used to sort points in ccw order about a pivot.
struct ccwSorter {
	const vec2& pivot;

	ccwSorter(const vec2& inPivot) : pivot(inPivot) { }

	bool operator()(const vec2& a, const vec2& b) {
		return ccw(pivot, a, b) < 0;
	}
};

// Calculation to see wether 'a' is ccw to 'b' or cw concering the pivot point.
// Starts at pivot, goes to 'a' & 'b' and checks if b is right from 'a'. If so, 'a' gets skipped. 
float ccw(const vec2& a, const vec2& b, const vec2& c) {
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}


// The convex hull is the smalles Polygon (veelhoek) which has all the given points within the lines.
vector<vec2> convex_hull(vector<vec2>& v) {
	// Points should be in the right ccw order.
	sort(v.begin() + 1, v.end(), ccwSorter(v[0]));

	// Return because we need to be able to check the next 2 points.
	if (v.size() < 4) return v;

	// Define hull & add first 3 points.
	vector<vec2> hull;
	auto it = v.begin();
	hull.push_back(*it++);
	hull.push_back(*it++);
	hull.push_back(*it++);

	// Iterate through the remaining points.
	while (it != v.end()) {
		// Check if the last 3 points make a left turn (ccw), if they don't it removes the last point of the hull until they do make a left turn or there are only 2 points left in the hull.
		while (ccw(*(hull.rbegin() + 1), *(hull.rbegin()), *it) >= 0 && hull.size() > 2) {
			hull.pop_back();
		}
		// After ensuring the last 3 points do make a left turn, it adds the point to the hull.
		hull.push_back(*it++);
	}

	return hull;
}