#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include <cmaths>

int main() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 1'000'000);

    std::vector<int> ints(100'000'000);
    for(auto& i : ints)
        i = dist(rd);
    
    std::vector<float> floats(ints.size()); 

    auto begin = std::begin(ints);
    auto end = std::end(ints);
    auto out_begin = std::begin(floats);
    std::size_t size = std::distance(begin, end);

    unsigned num_thread = std::thread::hardware_concurrency();
    if (num_thread == 0)
        num_thread = 1;

    if (size <= num_thread)
        num_thread = 1;

    std::vector<std::thread> threads;
    threads.reserve(num_thread);

    std::size_t element_exceeding = size % num_thread;
    std::size_t element_per_thread = (size - element_exceeding) / num_thread;
    std::cout << "Separated on " << num_thread << " threads, " << element_per_thread << " elements per thread with " << element_exceeding << " elements eceeding\n" << std::flush;

    auto thread_fun = [] (auto in, auto end, auto out) {
        for(; in != end; ++in, ++out)
            *out = std::sqrt(static_cast<float>(*in) / 2);
    };

    auto clock = std::chrono::system_clock::now();

    for(std::size_t i = 0; i < num_thread; ++i) {
        auto next = begin;
        std::advance(next, element_per_thread + (i < element_exceeding ? 1 : 0));

        if (i == num_thread - 1)
            next = end;

        std::cout << "Thread between " << std::distance(std::begin(ints), begin) << " and " << std::distance(std::begin(ints), next);
        std::cout << " (" << std::distance(begin, next) << " elements)\n";

        threads.emplace_back(thread_fun, begin, next, out_begin);

        begin = next;
        std::advance(out_begin, element_per_thread);
    }

    for(auto& thread : threads)
        thread.join();
    auto s = std::chrono::duration<double>(std::chrono::system_clock::now() - clock).count();

    std::cout << "done in " << s << "s" << std::endl;

    clock = std::chrono::system_clock::now();
    std::thread(thread_fun, std::begin(ints), std::end(ints), std::begin(floats)).join();
    s = std::chrono::duration<double>(std::chrono::system_clock::now() - clock).count();
    std::cout << "done in " << s << "s\n";

}