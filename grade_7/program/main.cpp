#include <iostream>
#include <pthread.h>
#include <chrono>

static long long honey_pot = 0;                                                             // A shared pot.
static long long total_capacity;                                                            // Shared pot capacity.
static int count_of_threads = 0;                                                            // Count of bees.
pthread_mutex_t mutex;                                                                      // Synchro-primitive.
static int bee_counter = 0;
static bool is_file_input = false;
static FILE *file_out = fopen("output.txt", "w");

void *fillHoneyPot(void *shared_arg) {
    int bee_number = ++bee_counter;
    auto local_capacity = ((long long) shared_arg) / count_of_threads;                      // Personal pot capacity.
    long long local_pot = 0;                                                                // Personal bee pot.

    // Filling the personal pot.
    for (long long i = 0; i < local_capacity; ++i) {
        ++local_pot;
    }

    pthread_mutex_lock(&mutex);
    // Transfusion of honey from the personal to the shared pot.
    honey_pot += local_pot;
    if (honey_pot != total_capacity && honey_pot == local_capacity * count_of_threads) {
        // Last bee must finish filling the shared pot whether there is some place for honey in the shared pot.
        for (long long i = 0; i < total_capacity - local_capacity * count_of_threads; ++i) {
            ++honey_pot;
        }
    }
    if (honey_pot == total_capacity) {
        std::cout << "bee #"  << bee_number << " called teddy bear!" << std::endl;
        std::cout << "bear started eating honey..." << std::endl;

        std::string outline = "bee #" + std::to_string(bee_number) + " called teddy bear!\n";
        fprintf(file_out, "%s", outline.c_str());
        outline = "bear started eating honey...\n\n";
        fprintf(file_out, "%s", outline.c_str());
    }
    pthread_mutex_unlock(&mutex);

    return nullptr;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "press 0 for console input, press other number for file input: ";
        std::cin >> is_file_input;
        if (is_file_input) {
            FILE *file_in = fopen("input.txt", "r");

            fscanf(file_in, "%d", &count_of_threads);
            if (count_of_threads < 1 || count_of_threads > 10) {
                std::cout << "incorrect count of bees..." << std::endl;
                fclose(file_in);
                return 0;
            }

            fscanf(file_in, "%lld", &total_capacity);
            if (total_capacity < 10 || total_capacity > 100000000000) {
                std::cout << "incorrect size of honey pot..." << std::endl;
                fclose(file_in);
                return 0;
            }

            fclose(file_in);
        } else {
            do {
                std::cout << "select count of bees in range [1;10]: ";
                std::cin >> count_of_threads;                                                   // Count of bees.
            } while (count_of_threads < 1 || count_of_threads > 10);

            std::cout << std::endl;

            do {
                std::cout << "select size of honey pot in range [10; 100.000.000.000]: ";
                std::cin >> total_capacity;                                                     // Shared pot capacity.
            } while (total_capacity < 10 || total_capacity > 100000000000);
        }
    } else if (argc == 3) {
        count_of_threads = atoi(argv[1]);
        if (count_of_threads < 1 || count_of_threads > 10) {
            std::cout << "incorrect count of bees..." << std::endl;
            return 0;
        }

        total_capacity = _atoi64(argv[2]);
        if (total_capacity < 10 || total_capacity > 100000000000) {
            std::cout << "incorrect size of honey pot..." << std::endl;
            return 0;
        }
    } else {
        std::cout << "incorrect argument line..." << std::endl;
        return 0;
    }

    std::cout << std::endl;

    pthread_t bees[count_of_threads];                                                   // A static array of threads.

    for (int i = 0; i < count_of_threads; ++i) {
        pthread_create(&bees[i], NULL, fillHoneyPot, (void *) total_capacity);          // Calling function for every thread of the array.
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();     // Start.
    for (int i = 0; i < count_of_threads; ++i) {
        pthread_join(bees[i], NULL);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();       // Finish.

    std::cout << std::endl;

    std::cout << "teddy bear has eaten all honey... :P" << std::endl;
    std::cout << "bear finished eating honey..." << std::endl;

    std::cout << std::endl;

    std::cout << "load time: "
              << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[sec]"
              << std::endl;
    std::cout << "total honey: " << honey_pot << std::endl;                             // Correctness checker.

    fprintf(file_out, "%s", "teddy bear has eaten all honey... :P\n");
    fprintf(file_out, "%s", "bear finished eating honey...\n\n");

    std::string outline = "load time: " +
                          std::to_string(std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()) +
                          "[sec]\n";
    fprintf(file_out, "%s", outline.c_str());
    outline = "total honey: " + std::to_string(honey_pot) + "\n";
    fprintf(file_out, "%s", outline.c_str());

    fclose(file_out);

    return 0;
}