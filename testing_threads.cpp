#include "precomp.h" // include (only) this in every .cpp file

int sumVector(const std::vector<int>& vec) {
    int sum = 0;
    for (int num : vec) {
        sum += num;
    }
    return sum;
}

void method_normal() {
    const int VECTOR_SIZE = 100000000;  // Large vector size
    std::vector<int> data(VECTOR_SIZE, 1);  // Initialize vector with all elements as 1

    auto start = std::chrono::high_resolution_clock::now();

    // Non-threaded version
    int result = sumVector(data);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
}

int sumVectorRange(const std::vector<int>& vec, std::size_t start, std::size_t end) {
    int sum = 0;
    for (std::size_t i = start; i < end; ++i) {
        sum += vec[i];
    }
    return sum;
}

void method_threaded(BS::thread_pool& pool) {
    const int VECTOR_SIZE = 100000000;  // Large vector size
    std::vector<int> data(VECTOR_SIZE, 1);  // Initialize vector with all elements as 1

    std::vector<std::thread> threads;
    std::vector<int> partialSums(NUM_THREADS, 0);

    auto start = std::chrono::high_resolution_clock::now();

    // Threaded version
    for (int i = 0; i < NUM_THREADS; ++i) {
        std::size_t startIdx = i * (VECTOR_SIZE / NUM_THREADS);
        std::size_t endIdx = (i == NUM_THREADS - 1) ? VECTOR_SIZE : (i + 1) * (VECTOR_SIZE / NUM_THREADS);

        pool.push_task([&, startIdx, endIdx, i]() {
            partialSums[i] = sumVectorRange(data, startIdx, endIdx);
            });
    }

    // Wait for all threads to finish
    pool.wait_for_tasks();

    // Combine partial sums
    int result = 0;
    for (int partialSum : partialSums) {
        result += partialSum;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Time taken (THREADED): " << duration.count() << " seconds" << std::endl;

}