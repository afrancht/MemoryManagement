# A memory allocator

A heap memory model based on memory control blocks using memory (char) arithmetic with a Worst Fit allocation algorithm.

# Classes

The file `heap.h` contains the `MemControlBlock` class.  Objects of this type are placed before used/available blocks of memory on the Heap, and are doubly-linked: each `MemControlBlock` has a pointer to the next and preceding blocks.  (For the head of the list, previous is `nullptr`; for the tail of the list, next is `nullptr`.)

The file `heap.h` is an implementation of a class `Heap`.  This mimics the Heap memory -- it reserves a block of memory by making an array of chars (bytes); then places a `MemControlBlock` at the start of this block using placement new.

# Methods

`char * allocateMemoryWorstFit(size_t requested)`  

This function is used to allocate blocks of memory for storing objects.  It takes a memory allocation request (a number of bytes -- `size_t` is another name for an unsigned integer) and returns a pointer to where the object should be created. The algorithm ensures alignment by rounding the requested amount to a multiple of 4 bytes, uses the worst fit algorithm (will return nullptr if there's no suitable block of memory available) to allocate memory and marks the block as not available. It will then calculate the remaining space. If space suffices it will create a new memory control block and set the size to the available space after this. It will update the previous and next pointers to include the block in the correct position and will update the size of the current memory control block. It will return the address of where the memory allocated starts (after the MCB).

`void deallocateMemory(char * toDeallocate)`

Deletes the memory and its memory control block, it then sets the address as being available and if the previous MCB is available it will then merge it. If the next is also available it will merge it as well.

# Running code

To test the code, use `TestHeap.cc`.  To compile this, and ensure your compiler is producing 32-bit code, run:

`g++ -std=c++11 -m32 -g -o TestHeap TestHeap.cc`

(The `-m32` argument sets it to 32-bits.)
