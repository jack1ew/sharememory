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
    // Create shared memory segment
    key_t key = ftok("shmfile", 65); // Generate unique key
    if (key == -1) {
        std::cerr << "Error generating shared memory key" << std::endl;
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(key, SHMSIZE, 0666|IPC_CREAT); // Create shared memory segment
    if (shmid == -1) {
        std::cerr << "Error creating shared memory segment" << std::endl;
        exit(EXIT_FAILURE);
    }

    char* shared_memory = (char*)shmat(shmid, nullptr, 0); // Attach shared memory segment
    if (shared_memory == (char*)-1) {
        std::cerr << "Error attaching shared memory segment" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (*shared_memory == '\0') {
        sleep(1);
    }
    // Read file name and path from shared memory
    std::string file_name_path(shared_memory);
    std::cout << "Server received file name and path: " << file_name_path << std::endl;

    // Open file and read contents
    std::ifstream file(file_name_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string file_contents((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

    // Write file contents to shared memory
    strncpy(shared_memory, file_contents.c_str(), SHMSIZE);

    std::cout << "Server wrote file contents to shared memory" << std::endl;
    *shared_memory = '#';
    // Wait for client to read file contents from shared memory
    while (*shared_memory != '*') {
        sleep(1);
    }

    // Detach and remove shared memory segment
    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, nullptr);

    return 0;
}
