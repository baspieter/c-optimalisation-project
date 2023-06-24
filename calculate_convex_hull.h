#pragma once

#ifndef CALCULATE_CONVEX_HULL_H
#define CALCULATE_CONVEX_HULL_H

vector<vec2> GrahamScan(vector<vec2> v);
bool isLeftOf(const vec2& a, const vec2& b);
struct ccwSorter;
float ccw(const vec2& a, const vec2& b, const vec2& c);

#endif