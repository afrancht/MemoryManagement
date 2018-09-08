#ifndef HEAP_H
#define HEAP_H

#include <cstddef>
#include <new>

#include <iostream>
using std::cout;
using std::endl;

/** @Author Alex FT */
class MemControlBlock {

public:

	/** True if the MemControlBlock is before a block of available memory */
	bool available;

	/** The size of the following block of memory, in bytes */
	size_t size;

	MemControlBlock * previous;
	MemControlBlock * next;

	MemControlBlock(const bool availableIn, const int sizeIn)
	: available(availableIn), size(sizeIn),
	previous(nullptr), next(nullptr) {
	}

	~MemControlBlock() {

		delete previous;
		delete next;
	}

};

class Heap {

private:
	char * const memory;

	/** The first MemControlBlock for the heap -- the head of the list */
	MemControlBlock * startOfHeap;

public:

	/** Heap destructor */
	~Heap() {
		delete [] memory;
	}

	// no copy constructor, move constructor, assignment operator, ...
	Heap(const Heap &) = delete;
	Heap(Heap &&) = delete;
	Heap & operator=(const Heap &) = delete;
	Heap & operator=(Heap &&) = delete;

	Heap(const size_t sizeIn)
	: memory(new char[sizeIn]) {

		// make a MemControlBlock at the start of the reserved memory
		startOfHeap = new(memory) MemControlBlock(// true = is available
			true,
			// size = the size of the heap - the size of the MemControlBlock a the start of the heap
			sizeIn - sizeof(MemControlBlock)
		);
	}

	/** Used for debugging - get the address of the start of the heap */
	char* getStartOfHeap() const {
		return memory;
	}

	/** Used for debugging -- print out the details of the MemControlBlocks on the heap */
	void print() const {
		MemControlBlock * curr = startOfHeap;

		for (int i = 0; curr; ++i, curr = curr->next) {
			cout << "Block " << i << ": ";
			if (curr->available) {
				cout << "is free, ";
			} else {
				cout << " is in use, ";
			}
			cout << "size: " << curr->size << " bytes\n";
		}

	}


	/** Request a block of memory of the given size
	*
	* Uses the 'Worst Fit' algorithm to choose a suitable block of available memory
	*
	* Returns the memory address of the start of the requested memory
	* If no block is big enough, it returns nullptr.
	*/
	char * allocateMemoryWorstFit(size_t requested) {
		int sizeToAllocate = rounder(requested);


		MemControlBlock * memHole = worstFitFinder(sizeToAllocate);

		if ( memHole->next == nullptr && sizeToAllocate == memHole->size ) {

			memHole->size += sizeToAllocate;
			memHole->available = false;
			return ( reinterpret_cast<char*>(memHole) + sizeof(MemControlBlock) );
		}

		if (memHole == nullptr) {
			return nullptr;

		} else {

			// Mark memHole as not available
			memHole->available = false;
			int sizeOf = sizeof(MemControlBlock);

			int spareSpace = memHole->size - sizeToAllocate;

			if ( spareSpace >= sizeOf ) {

				/** Make a new MemControlBlock after the address of the chosen block,
				* plus the size of a MemControlBlock, plus the requested amount. Change
				* the type to char so we can do memory arithmetic
				*/
				auto x = reinterpret_cast<char*>(memHole);

				// Address of the new control block = address of the prev + size of a CB + space needed.
				auto newMCBAddress = x + sizeof(MemControlBlock) + sizeToAllocate;

				// Memory of the new control block = spare space - sizeof MCB
				auto newMemBlockSize = spareSpace - sizeof(MemControlBlock);

				MemControlBlock* newOne = new(newMCBAddress) MemControlBlock(true,newMemBlockSize);

				if ( memHole->next != nullptr) {
					memHole->next->previous = newOne;
				}
				newOne->previous = memHole;
				newOne->next = memHole->next;
				memHole->next = newOne;
				memHole->size = memHole->size - newOne->size - sizeOf;
				return x + sizeof(MemControlBlock);
			}
		}

		return nullptr;
	}

	size_t rounder(size_t requestedToRound) {

		size_t rounded = requestedToRound;

		if ( rounded%4 != 0) {
			rounded = rounded + ( 4 - (rounded%4));
		}
		return rounded;
	}

	MemControlBlock * worstFitFinder(size_t requestedRounded) {

		if ( startOfHeap->next == nullptr) {
			return startOfHeap;
		}

		MemControlBlock* curr = startOfHeap;
		MemControlBlock* biggest = curr;


		// while we're not at the end of the linked list.
		while (curr != nullptr) {

			while ( !biggest->available && biggest != nullptr ) {
				biggest = biggest->next;
			}

			if ( biggest == nullptr ) {
				return nullptr;
			}

			// if current is avaiable, the memory requested is smaller or equal to the available and its the largest size so far.
			if ( curr->available && curr->size >= requestedRounded && curr->size >= biggest->size  ) {


				biggest = curr;

			}
			curr = curr->next;
		}

		// if biggest is not avaialable then the last if did not occur so it's the start of the heap. If that is not available then nullptr.
		if ( !biggest->available ) {
			return nullptr;

		} else {

			return biggest;
		}

	}

	/** Deallocate the memory used by the object at the given address */
	void deallocateMemory(char * toDeallocate) {
		char * memoryHolder = reinterpret_cast<char*>(toDeallocate - sizeof(MemControlBlock));
		MemControlBlock* curMCB = reinterpret_cast<MemControlBlock*>(memoryHolder);

		curMCB->available = true;
		if (curMCB->previous != nullptr && curMCB->previous->available) {

			curMCB->previous->size = curMCB->previous->size + sizeof(MemControlBlock) + curMCB->size;

			curMCB->previous->next = curMCB->next;
			if (curMCB->next != nullptr) curMCB->next->previous = curMCB->previous;

		}

		if ( curMCB->next != nullptr && curMCB->next->available ) {
			curMCB->previous->size = 	curMCB->previous->size + sizeof(MemControlBlock) + curMCB->next->size;
			curMCB->previous->next = curMCB->next->next;
			curMCB->next->next->previous = curMCB->previous;
			curMCB->next->next = nullptr;
			curMCB->next->previous = nullptr;

		}

		curMCB -> previous = nullptr;
		curMCB -> next = nullptr;

		curMCB->~MemControlBlock();

	}

};

#endif
