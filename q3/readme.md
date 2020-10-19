
The basic logic behind this code is the usage of 5 semaphores.
1. astage: Acoustic stages (initial value=a)
2. estage: Electric stages (initial value=e)
3. stage: All stages (initial value=a+e)
4. sstage: Stages that singers can perform on (Solo or with a Musician) (initial value=a+e)
5. coord: Coordinators who hand out T-shirts (initial value=c)

Everytime a musician/singer occupies a stage, the value of the corresponding semaphore(s) decreases by one, and increases when the stage is ready to use again.
I've used two structs, pe and st, to store the details of performers and stages respectively. 
 - For the performer, I store name, instrument, arrival time, maximum waiting time and stage number. The details are stored in array perf[]. 
 - For stage, I've stored name of main performer, name of accompanying singer if any, instrument, type of stage, number of performers, mutex for singer and mutex for the stage itself. Mutex locks are used to ensure that a stage is not claimed by 2 performers. Note that when there are a total of (a+e) stages, the first 'a' stages are designated as acoustic stages and remaining 'e' stages are designated as electric stages. This is an assumption made. Details of all stages are stored in an array stages[]. 

Initially, all the performers(threads) go to srujana and arrive based on their arrival time. In the function srujana, I've picked or waited for a suitable stage based on the instrument character(in case of singing the character is s). The following logic is used to enter stages:
1. A musician entering/exiting an astage also enters/exits a stage.
2. A musician entering/exiting an estage also enters/exits a stage.
3. A singer entering/exitting an sstage also enters/exits a stage as well as either an astage or an estage depending on what stage it was.
An acoustic performer looks for astage, an electric performer looks for estage, and a performer that can perform in both just looks for a stage. A singer looks for an sstage, which is a stage either fully free or occupied by a musician that he/she can join.

A performer has a maximum time for which it waits for a suitable stage, before leaving. This has been implemented using sem_timedwait(). We make them wait until a point in time T=(Point in time of arrival+Maximum waiting time).
The musician performs for a random time between given limits(input). If a singer also joins him/her, this gets extended by 2 seconds.
Upon leaving, every musician and singer will receive a Tshirt from a coordinator. This is done by using the semaphore coord.

In the end, all the threads are joined back, and mutexes and semaphores are destroyed

BONUSES:
1. Every singer gets a T-shirt at the end of either a solo or a joined performance. I have assumed that in case of a joined performance, the singer will get the T-shirt first, followed by the musician. However, both of them will get T-shirts.
2. Stages are numbered from 1 to N where N is the total number of stages such that it is equal to the sum of acoustic and electric stages (a+e). When we display information about a performer performing on a stage or ending their performance on a stage, we also mention the stage number.