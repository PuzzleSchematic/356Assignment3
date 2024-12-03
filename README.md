ECE 356 Assignment 3
-----

Question 1
-----

airport_deadlock_prone.c

#This program demonstrates an unregulated airport; airplanes locking the Control Tower and Runway semaphores in an unordered fashion leading to potential deadlocks.
#An output can be seen in Figure 1.

airport_deadlock_free.c

#This program demonstrates a regulated airport; airplanes now lock semapohores for the Control Tower, then the Runway.
#Employing this order, removes any circular waiting potential, and all planes are able to take off/land without deadlock.
#An output can be see in Figure 2.
