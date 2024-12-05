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

Question 2
-----

chat_server.c & chat_client.c

These programs work in tandem with each other, simulating a multi-client chat application using socket programming. The server acts as a central hub facilitating communication between the cleints, allowing them to exchange messages.
The server was initialized (using socket()) on a designated port 5001 (using bind()) and listens for incoming clients (using listen() to listen and accept() to accept new clients).
The client connects to the server using its IP address and port number (using connect()). Once connected the client can send messges to the server which then receives (using recv()) and broadcasts the message to all other connected clients.
Each of the messages sent from the clients to the server are uniquely tagged by the server indicating which client the message is from.
The server uses multithreading to handle multiple clients at once with each client assigned a unique thread. Mutexes are used to ensure thread-safe management of shared resoureces like the list of connected clients.
If a client disconnects, the server detects this and removes the client from the list, ensuring the other clients aren't affected.
