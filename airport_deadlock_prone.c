#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h> // For O_CREAT and O_EXCL
#include <string.h> // For semaphore names

sem_t* runways[5];
sem_t* control_tower;

int total_runways = 3;
int total_airplanes = 5;

void* airplane_action(void* arg) {
    int airplane_id = *(int*)arg;
    int runway_index = airplane_id % total_runways; // Choose a runway

    // Even airplanes lock runway first, Odd airplanes lock control tower first
    if (airplane_id % 2 == 0) {
        printf("Airplane %d is trying to lock Runway %d...\n", airplane_id, runway_index);
        sem_wait(runways[runway_index]);
        printf("Airplane %d locked Runway %d.\n", airplane_id, runway_index);

        printf("Airplane %d is trying to lock the Control Tower...\n", airplane_id);
        sem_wait(control_tower);
        printf("Airplane %d locked the Control Tower.\n", airplane_id);
    } else {
        printf("Airplane %d is trying to lock the Control Tower...\n", airplane_id);
        sem_wait(control_tower);
        printf("Airplane %d locked the Control Tower.\n", airplane_id);

        printf("Airplane %d is trying to lock Runway %d...\n", airplane_id, runway_index);
        sem_wait(runways[runway_index]);
        printf("Airplane %d locked Runway %d.\n", airplane_id);
    }

    // Simulate takeoff or landing
    printf("Airplane %d is taking off/landing on Runway %d...\n\n", airplane_id, runway_index);

    sleep(2); // Simulate time taken for the operation

    sem_post(control_tower); // Unlock control tower
    printf("Airplane %d released the Control Tower.\n", airplane_id);

    sem_post(runways[runway_index]); // Unlock the runway
    printf("Airplane %d released Runway %d.\n", airplane_id, runway_index);

    pthread_exit(NULL);
}

int main() {
    pthread_t airplanes[total_airplanes]; // 5 Airplanes
    int airplane_ids[total_airplanes];

    char sem_name[20];

    for (int i = 0; i < total_runways; i++) {
        sem_close(runways[i]);
        snprintf(sem_name, sizeof(sem_name), "/runway_%d", i);
        sem_unlink(sem_name);
    }

    sem_close(control_tower);
    sem_unlink("/control_tower");

    // Create named semaphores for runways
    for (int i = 0; i < total_runways; i++) {
        snprintf(sem_name, sizeof(sem_name), "/runway_%d", i);
        runways[i] = sem_open(sem_name, O_CREAT | O_EXCL, 0644, 1);
        if (runways[i] == SEM_FAILED) {
            perror("sem_open failed for runway");
            exit(EXIT_FAILURE);
        }
    }

    // Create a named semaphore for the control tower (sem_init is depricated on new MacOS)
    control_tower = sem_open("/control_tower", O_CREAT | O_EXCL, 0644, 1);
    if (control_tower == SEM_FAILED) {
        perror("sem_open failed for control tower");
        exit(EXIT_FAILURE);
    }

    // Airplane threads
    for (int i = 0; i < total_airplanes; i++) {
        airplane_ids[i] = i + 1;
        pthread_create(&airplanes[i], NULL, airplane_action, &airplane_ids[i]);
    }

    for (int i = 0; i < total_airplanes; i++) {
        pthread_join(airplanes[i], NULL);
    }

    //Let Simulation run, then close/unlink semaphores (sem_destroy is depricated on new MacOS)

    for (int i = 0; i < total_runways; i++) {
        sem_close(runways[i]);
        snprintf(sem_name, sizeof(sem_name), "/runway_%d", i);
        sem_unlink(sem_name);
    }

    sem_close(control_tower);
    sem_unlink("/control_tower");

    printf("Simulation completed.\n");
    return 0;
}