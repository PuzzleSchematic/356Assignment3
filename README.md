ECE 356 Assignment 3
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

This program demonstrates another regulated airport that also implements a fair queue; tracking waiting times for each airplane, letting the airplane with the longest wait time to go next.
The wait time was implemented by making the airplane a structure and providing it with a variable that shows how long the craft has been waiting.
There were 3 functions designed to help with this, search, enqueue, and dequeue;
Search went through the waiting queue and looked for the airplane with the longest wait time, returning the index in the queue with the longest wait time.
Enqueue puts the airplane into the queue with given their id, and provides the plane with a beginning waiting time.
Dequeue uses the index returned from Search and removes that plane from the queue, reindexing the other planes to fill the gap.
Once a plane leaves the queue, it is the only plane able take control of the Control Tower and runway they require.
This removes any posibilities of starvation.
An output can be seen in Figure 3.
