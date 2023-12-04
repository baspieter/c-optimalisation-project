#pragma once

#ifndef DRAW_HEALTH_BARS_H
#define DRAW_HEALTH_BARS_H

void initialize_team_health_bar(vector<Tank>& tanks, int team, Surface* screen);
vector<int> sort_tank_team_healths(vector<Tank>& tanks, int team);
void draw_health_bars(const std::vector<int>& sorted_healths, const int team, Surface* screen);

#endif