#ECE 356 Assignment 3
-----

Question 1
-----

airport_deadlock_prone.c

This program demonstrates an unregulated airport; airplanes locking the Control Tower and Runway semaphores in an unordered fashion leading to potential deadlocks.
An output can be seen in Figure 1.

airport_deadlock_free.c

This program demonstrates a regulated airport; airplanes now lock semapohores for the Control Tower, then the Runway.
Employing this order, removes any circular waiting potential, and all planes are able to take off/land without deadlock.
An output can be see in Figure 2.

airport_starvation_prevention.c

This program demonstrates another regulated airport that also implements a fair queue; tracking waiting times for each airplane, letting the airplane with the longest wait time to go next
The wait time was implemented by making the airplane a structure and providing it with a variable that shows how long the craft has been waiting
An output can be seen in Figure 3.
