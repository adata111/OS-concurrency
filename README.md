# OS-concurrency

To compile and run any file execute the following command:

```c
 gcc <filename>.c -lpthread
 ./a.out
```

## q1.c
Implementation of the merge sort algorithm in 3 ways: normally, using processes, and using threads. Runtime of all these methods are compared for the input given by user. 

## q2.c
### Back to College
A system that simulates companies producing vaccines and supplying them to vaccination zones which in vaccinate students arriving at random times.
Used mutexes and threads

## q3.c
### Musical Mayhem
A system that simulates a musical event with several types of performers - Pianist(p), Guitarist(g), Violinist(v), Bassist(b), Singer(s). All these performers perform on any one of two kinds of stage - acoustic or electric. After performance, every performer gets a tshirt from a coordinator. Timedwait has been used to account for the impatient performers who don't get a free stage before their patience exhausts.
Used semaphores, mutexes and threads
