#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

// New structure for Airplane
typedef struct {
    int id;
    time_t waiting_time; // Time when the airplane starts waiting
} Airplane;

sem_t* runways[3];
sem_t* control_tower;


Airplane queue[5]; // Waiting queue for up to 5 airplanes for this example
int queue_size = 0; // Initial queue size
pthread_mutex_t queue_lock; // Lock to protect the queue
pthread_mutex_t landing_takeoff_lock; // Lock to prevent more than 1 airplane from leaving the queue

int total_runways = 3;
int total_airplanes = 5;



// Function to search the queue for the longest waiting airplane
int search(){
    pthread_mutex_lock(&queue_lock);
    int highest_priority_index = 0;
    for (int i = 1; i < queue_size; i++) {
            if (queue[i].waiting_time < queue[highest_priority_index].waiting_time) {
                highest_priority_index = i;
            }
        }
    pthread_mutex_unlock(&queue_lock);
    return highest_priority_index;
}



// Function to add an airplane to the waiting queue
void enqueue(Airplane airplane) {
    pthread_mutex_lock(&queue_lock);

    queue[queue_size] = airplane;
    queue[queue_size].waiting_time = time(NULL); // Provide time airplane begins to wait
    queue_size++;

    pthread_mutex_unlock(&queue_lock);
}



// Function to remove the airplane with the highest priority (longest waiting time)
Airplane dequeue(int highest_priority_index) {
    pthread_mutex_lock(&queue_lock);

    // Remove THAT airplane from the queue and move the rest to fill the gap
    Airplane highest_priority_airplane = queue[highest_priority_index];
    for (int i = highest_priority_index; i < queue_size - 1; i++) {
        queue[i] = queue[i + 1];
    }
    queue_size--;

    pthread_mutex_unlock(&queue_lock);
    return highest_priority_airplane;
}




void* airplane_action(void* arg) {
    Airplane plane;
    plane.id = *(int*)arg;

    // Add the airplane to the waiting queue
    enqueue(plane);
    printf("Airplane %d added to the queue.\n", plane.id);

    // Wait until this airplane has the longest waiting time
    int highest_priority_index = 0;
    while (1) {

        // Find the airplane with the longest waiting time
        highest_priority_index = search();

        // If THIS airplane is has the longest waiting time
        if (queue_size > 0 && plane.id == queue[highest_priority_index].id) {
            // Remove the airplane from the queue
            dequeue(highest_priority_index);
            break;
        }

        usleep(1000); // Sleep briefly to avoid busy-waiting
    }

    pthread_mutex_lock(&landing_takeoff_lock);

    // Lock the control tower
    printf("Airplane %d is trying to lock the Control Tower...\n", plane.id);
    sem_wait(control_tower);
    printf("Airplane %d locked the Control Tower.\n", plane.id);

    // Lock the runway
    int runway_index = plane.id % total_runways;
    printf("Airplane %d is trying to lock Runway %d...\n", plane.id, runway_index);
    sem_wait(runways[runway_index]);
    printf("Airplane %d locked Runway %d.\n", plane.id, runway_index);

    // Simulate takeoff or landing
    printf("Airplane %d is taking off/landing on Runway %d...\n", plane.id, runway_index);
    sleep(2); // Simulate operation

    // Release resources
    sem_post(runways[runway_index]);
    printf("Airplane %d released Runway %d.\n", plane.id, runway_index);

    sem_post(control_tower);
    printf("Airplane %d released the Control Tower.\n\n", plane.id);

    pthread_mutex_unlock(&landing_takeoff_lock);

    pthread_exit(NULL);
}

int main() {
    pthread_t airplanes[total_airplanes];
    int airplane_ids[total_airplanes];

    char sem_name[20];

    pthread_mutex_destroy(&landing_takeoff_lock);

    for (int i = 0; i < total_runways; i++) {
        sem_close(runways[i]);
        snprintf(sem_name, sizeof(sem_name), "/runway_%d", i);
        sem_unlink(sem_name);
    }

    sem_close(control_tower);
    sem_unlink("/control_tower");

    pthread_mutex_init(&landing_takeoff_lock, NULL);


    // Create named semaphores for runways (sem_init is depricated on new MacOS)
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

    // Create airplane threads
    for (int i = 0; i < total_airplanes; i++) {
        airplane_ids[i] = i + 1;
        pthread_create(&airplanes[i], NULL, airplane_action, &airplane_ids[i]);
    }

    // Wait for all airplane threads to finish
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