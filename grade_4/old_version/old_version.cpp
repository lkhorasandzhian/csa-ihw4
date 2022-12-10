#include <iostream>
#include <pthread.h>
#include <chrono>

static long long honey_pot = 0;
pthread_mutex_t mutex;

[[deprecated("current logic is even slower than the new one")]]
void *fillHoneyPot(void *shared_arg) {
    auto capacity = (long long) shared_arg;

    while (true) {
        pthread_mutex_lock(&mutex);
        if (honey_pot == capacity) {
            pthread_mutex_unlock(&mutex);
            return nullptr;
        }
        ++honey_pot;
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    int n;
    do {
        std::cout << "select count of bees in range [1;10]: ";
        std::cin >> n;
    } while (n < 1 || n > 10);

    std::cout << std::endl;

    long long capacity;
    do {
        std::cout << "select size of honey pot in range [10; 100.000.000.000]: ";
        std::cin >> capacity;
    } while (capacity < 10 || capacity > 100000000000);

    std::cout << std::endl;

    pthread_t bees[n];

    for (int i = 0; i < n; ++i) {
        pthread_create(&bees[i], NULL, fillHoneyPot, (void *) capacity);
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // Start.
    fillHoneyPot((void *)capacity);
    for (int i = 0; i < n; ++i) {
        pthread_join(bees[i], NULL);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();    // Finish.

    std::cout << "teddy bear has eaten all honey... :P" << std::endl;
    std::cout << "Bear finished eating honey..." << std::endl;

    std::cout << "Load time: "
              << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[sec]"
              << std::endl;
    std::cout << "total honey: " << honey_pot << std::endl;

    honey_pot = 0;

    return 0;
}
