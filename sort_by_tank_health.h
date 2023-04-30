#pragma once

#ifndef SORT_BY_TANK_HEALTH_H
#define SORT_BY_TANK_HEALTH_H

int partition(vector<Tank>& tanks, int start, int end);
void sort_tanks_by_health(vector<int>& healths, int start, int end);

#endif