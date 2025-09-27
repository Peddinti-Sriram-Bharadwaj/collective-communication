#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>

// Using a smaller iteration count because we're launching many threads
// A single, templated function that can work with either struct type.
// This ensures the logic for the work is identical in both tests.
template <typename T>
void increment(T* counter) {
    for (long long i = 0; i < ITERATIONS; ++i) {
        counter->value++;
    }
}

int main() {
    // Get the number of logical cores to saturate the CPU
    unsigned int core_count = std::thread::hardware_concurrency();
    std::cout << "Detected " << core_count << " logical cores." << std::endl;
    std::cout << "Running " << ITERATIONS << " iterations per thread." << std::endl;

    // --- Time the BAD version ---
    // Using the unaligned struct, inviting false sharing.
    std::vector<UnalignedCounter> bad_counters(core_count);
    std::vector<std::thread> bad_threads;

    auto start_bad = std::chrono::high_resolution_clock::now();
    for (unsigned int i = 0; i < core_count; ++i) {
        bad_threads.emplace_back(increment<UnalignedCounter>, &bad_counters[i]);
    }
    for (auto& t : bad_threads) {
        t.join();
    }
    auto end_bad = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_bad = end_bad - start_bad;

    std::cout << "\n--- Bad Version (Unaligned Struct) ---" << std::endl;
    std::cout << "Time elapsed: " << duration_bad.count() << " seconds" << std::endl;

    // --- Time the GOOD version ---
    // Using the aligned struct to prevent false sharing.
    std::vector<AlignedCounter> good_counters(core_count);
    std::vector<std::thread> good_threads;

    auto start_good = std::chrono::high_resolution_clock::now();
    for (unsigned int i = 0; i < core_count; ++i) {
        good_threads.emplace_back(increment<AlignedCounter>, &good_counters[i]);
    }
    for (auto& t : good_threads) {
        t.join();
    }
    auto end_good = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_good = end_good - start_good;

    std::cout << "\n--- Good Version (Aligned Struct) ---" << std::endl;
    std::cout << "Time elapsed: " << duration_good.count() << " seconds" << std::endl;

    return 0;
}
