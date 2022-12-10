#include <iostream>
#include <pthread.h>
#include <chrono>

static long long honey_pot = 0;
static long long total_capacity;
static int count_of_threads = 0;
pthread_mutex_t mutex;

void *fillHoneyPot(void *shared_arg) {
    auto local_capacity = ((long long) shared_arg) / count_of_threads;
    long long local_pot = 0;

    for (long long i = 0; i < local_capacity; ++i) {
        ++local_pot;
    }

    pthread_mutex_lock(&mutex);
    honey_pot += local_pot;
    if (honey_pot == total_capacity) {
    } else if (honey_pot == local_capacity * count_of_threads) {
        for (long long i = 0; i < total_capacity - local_capacity * count_of_threads; ++i) {
            ++honey_pot;
        }
    }
    pthread_mutex_unlock(&mutex);

    return nullptr;
}

int main() {
    do {
        std::cout << "select count of bees in range [1;10]: ";
        std::cin >> count_of_threads;
    } while (count_of_threads < 1 || count_of_threads > 10);

    std::cout << std::endl;

    do {
        std::cout << "select size of honey pot in range [10; 100.000.000.000]: ";
        std::cin >> total_capacity;
    } while (total_capacity < 10 || total_capacity > 100000000000);

    std::cout << std::endl;

    pthread_t bees[count_of_threads];

    for (int i = 0; i < count_of_threads; ++i) {
        pthread_create(&bees[i], NULL, fillHoneyPot, (void *) total_capacity);
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // Start.
    for (int i = 0; i < count_of_threads; ++i) {
        pthread_join(bees[i], NULL);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();    // Finish.

    std::cout << "teddy bear has eaten all honey... :P" << std::endl;
    std::cout << "bear finished eating honey..." << std::endl;

    std::cout << "load time: "
              << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[sec]"
              << std::endl;
    std::cout << "total honey: " << honey_pot << std::endl;

    return 0;
}
