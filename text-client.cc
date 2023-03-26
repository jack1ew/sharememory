#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>

const int SHMSIZE = 1024;
const char* SHMNAME = "SharedMemory";
const char* SEMNAME = "Semaphore";

int main() {
    // Create shared memory segment
    int shared_memory_id = shmget(ftok(SHMNAME, 1), SHMSIZE, 0666 | IPC_CREAT);
    if (shared_memory_id < 0) {
        std::cerr << "Error creating shared memory segment" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Attach shared memory segment
    char* shared_memory = (char*)shmat(shared_memory_id, NULL, 0);
    if (shared_memory == (char*)-1) {
        std::cerr << "Error attaching shared memory segment" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Open semaphore
    sem_t* semaphore = sem_open(SEMNAME, O_CREAT, 0666, 0); // Create new semaphore
    if (semaphore == SEM_FAILED) {
        std::cerr << "Error opening semaphore" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Write file name and path to shared memory
    std::string file_name_path;
    std::cout << "Enter file name and path: ";
    std::getline(std::cin, file_name_path);

    strncpy(shared_memory, file_name_path.c_str(), SHMSIZE);

    // Signal server to read file name and path
    sem_post(semaphore);

    // Wait for server to write file contents to shared memory
    sem_wait(semaphore);

    // Read file contents from shared memory
    char buffer[SHMSIZE];
    strncpy(buffer, shared_memory, SHMSIZE);

    // Output file contents
    std::cout << buffer << std::endl;

    // Detach shared memory segment
    shmdt(shared_memory);

    // Close semaphore
    sem_close(semaphore);

    return 0;
}

