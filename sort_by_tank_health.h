#pragma once

#ifndef SORT_BY_TANK_HEALTH_H
#define SORT_BY_TANK_HEALTH_H

int Partition(vector<const Tank*>& tanks, int start, int end);
void Quicksort(vector<const Tank*>& tanks, int start, int end);
Tank* getPointer(Tank& tank);

#endif