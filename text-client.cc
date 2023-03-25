#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fstream>

#define SHMSIZE 1024 // Size of shared memory

int main() {
    // Get shared memory segment
    key_t key = ftok("shmfile", 65); // Generate unique key
    if (key == -1) {
        std::cerr << "Error generating shared memory key" << std::endl;
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(key, SHMSIZE, 0666); // Get shared memory segment
    if (shmid == -1) {
        std::cerr << "Error getting shared memory segment" << std::endl;
        exit(EXIT_FAILURE);
    }

    char* shared_memory = (char*)shmat(shmid, nullptr, 0); // Attach shared memory segment
    if (shared_memory == (char*)-1) {
        std::cerr << "Error attaching shared memory segment" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Write file name and path to shared memory
    const char* file_name_path = "bankloan1.csv"; // Replace with actual file path
    strncpy(shared_memory, file_name_path, SHMSIZE);

    std::cout << "Client wrote file name and path to shared memory" << std::endl;
    *shared_memory = '\0';
    // Wait for server to write file contents to shared memory
    while (*shared_memory != '#') {
        sleep(1);
    }

    // Read file contents from shared memory
    std::string file_contents(shared_memory);

    // Print file contents
    std::cout << "File contents:" << std::endl;
    std::cout << file_contents << std::endl;
    *share_memory = '*';
    // Detach shared memory segment
    shmdt(shared_memory);

    return 0;
}
