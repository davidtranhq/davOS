#include <array>
#include <iostream>
#include <format>
#include <semaphore>
#include <unordered_set>
#include <thread>
#include <vector>

template <typename T, size_t N>
class ConcurrentQueue {
public:
    template <typename U>
    auto push(U &&value) -> void {
        can_push.acquire();
        queue_lock.acquire();
        queue[back] = value;
        back = (back + 1) % N;
        queue_lock.release();
        can_pop.release();
    }

    auto pop() -> T {
        can_pop.acquire();
        queue_lock.acquire();
        auto value = queue[front];
        front = (front + 1) % N;
        queue_lock.release();
        can_push.release();
        return value;
    }

private:
    std::array<T, N> queue {};
    std::size_t front {0}, back {0};
    std::binary_semaphore queue_lock {1};
    std::counting_semaphore<N> can_push {N}, can_pop {0};
};

void test_multiple_threads() {
    std::cout << "Running multiple threads test...";
    constexpr auto queue_size = 100;
    constexpr auto num_threads = 10;
    constexpr auto values_per_thread = 1000;

    auto queue = ConcurrentQueue<int, queue_size> {};
    auto threads = std::vector<std::thread> {};

    for (auto i = 0; i < num_threads; ++i) {
        threads.emplace_back([&queue, i] {
            for (int j = 0; j < values_per_thread; ++j) {
                queue.push(i * values_per_thread + j);
            }
        });
    }

    auto popped_values = std::vector<int> {};
    auto mutex = std::mutex {};
    for (auto i = 0; i < num_threads; ++i) {
        threads.emplace_back([&mutex, &queue, &popped_values] {
            for (int j = 0; j < values_per_thread; ++j) {
                auto lock = std::lock_guard<std::mutex> {mutex};
                popped_values.push_back(queue.pop());
            }
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    auto popped_set = std::unordered_set<int> {popped_values.begin(), popped_values.end()};
    auto constexpr expected_unique_values = num_threads * values_per_thread;
    if (popped_set.size() == num_threads * values_per_thread) {
        std::cout << "[PASS]\n";
    } else {
        std::cout << std::format("[FAIL]\nExpected {} unique values, got {}\n",
            expected_unique_values, popped_set.size());
    }
}

int main() {
    test_multiple_threads();
    return 0;
}