Given a number n followed by n numbers, we sort them by using Merge Sort in 3 different ways: Normal, Concurrent(multiple processes), Threaded. In all 3 cases, a selection sort is performed when the array being considered at any stage has less than 5 elements.

In concurrent version, we use forking, and so a hierarchy exists in the form of parent and child. A shared memory is used by all.
In threaded version, instead of processes we use threads.

We find that in every case for number of elements more than 5, Normal 
Mergesort is faster than Concurrent Mergesort and Threaded Mergesort.
Concurrent mergesort is accessing left half array and right half array
simultaneously. When we access left half array, left half array is loaded 
into cache memory. Simultaneously, it tries to access right half array then cache miss will happen . Multiple cache misses like this make it run slower.
In simple mergesort, we access a few elements of the left half array and 
right half array at a time. By doing so, we ensure that those elements stay 
in cache and the time taken to retreive them is much lesser than the time 
taken to retreive them in the concurrent case. Threaded mergesort takes more
time due to the creation of multiple threads.