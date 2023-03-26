#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <fstream>

#define SHMSIZE 1024 // Size of shared memory
#define SEMNAME "/mysemaphore" // Name of semaphore

int main() {
    // Create shared memory segment
    key_t key = ftok("shmfile.c", 65); // Generate unique key
    if (key == -1) {
        std::cerr << "Error generating shared memory key" << std::endl;
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(key, SHMSIZE, IPC_CREAT | 0666); // Create shared memory segment
    if (shmid == -1) {
        std::cerr << "Error creating shared memory segment" << std::endl;
        exit(EXIT_FAILURE);
    }

    char* shared_memory = (char*)shmat(shmid, nullptr, 0); // Attach shared memory segment
    if (shared_memory == (char*)-1) {
        std::cerr << "Error attaching shared memory segment" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create semaphore
    sem_t* semaphore = sem_open(SEMNAME, O_CREAT | O_EXCL, 0666, 0); // Initialize to 0
    if (semaphore == SEM_FAILED) {
        std::cerr << "Error creating semaphore" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Wait for client to write file name and path to shared memory
    while (*shared_memory == '\0') {
        sleep(1);
    }

    // Read file name and path from shared memory
    std::string file_name_path(shared_memory);

    // Open file for reading
    std::ifstream file(file_name_path);

    // Read file contents into buffer
    char buffer[SHMSIZE];
    file.read(buffer, SHMSIZE);

    // Write file contents to shared memory
    strncpy(shared_memory, buffer, SHMSIZE);

    // Signal client that file contents have been written
    sem_post(semaphore);

    // Detach shared memory segment
    shmdt(shared_memory);

    // Delete shared memory segment
    shmctl(shmid, IPC_RMID, nullptr);

    // Close semaphore
    sem_close(semaphore);

    // Unlink semaphore
    sem_unlink(SEMNAME);

    return 0;
}


