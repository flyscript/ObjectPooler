# ObjectPooler
A lightning-fast, self-managing, and efficient pooling system I created as part of my Bachelors

This system is templated, for use with any class, uses a single self-managing table of fixed size - defined when the pool is initiated - to control the use of objects inside the pool; both retriving and releasing, with a simple and efficient pointer system that draws the line between active and available objects.

The system also has the ability to reallocate memory for itself _if_ you request it with size(int a_iNewSize); to change the number of objects in the pool

Of course, it also has the essential abilities to get the next object, release an object back to the pool, retrive the size of the pool, retrive the number of available objects, retrive the number of active objects, and retrive all currently active objects.
