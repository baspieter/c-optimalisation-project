#pragma once

#include "BS_thread_pool.hpp"

#ifndef TESTING_THREADS
#define TESTING_THREADS

void method_normal();
int sumVector(const std::vector<int>& vec);
void method_threaded(BS::thread_pool& pool);
int sumVectorRange(const std::vector<int>& vec, std::size_t start, std::size_t end);

#endif