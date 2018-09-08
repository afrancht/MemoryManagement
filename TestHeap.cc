#include "heap.h"

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

class TestObject {

public:
    
    int x;
    int y;
    
    TestObject() : x(0), y(1) {}
    
};

int main() {
    
    {
        Heap heap(1048576);
    
        size_t bigRequest = (1048576 - sizeof(MemControlBlock)) / 2;
        
        cout << "--------------------------------------------------------------------------------------\n";
        cout << "Test 0: heap of size 1048576; then requesting space for two objects of size " << bigRequest << "\n";
        cout << "(the second request should fail)\n";
        cout << "\nMaking first allocation request";
        cout.flush();
        
        auto addrOne = heap.allocateMemoryWorstFit(bigRequest);
                
        if (addrOne == nullptr) {
            cout << " - Failed: returned nullptr\n";
            cout << "  * heap.print() gives\n";
            heap.print();
        } else {
            char * shouldBe = heap.getStartOfHeap() + sizeof(MemControlBlock);
            if (shouldBe == addrOne) {
                cout << " - Passed: object created at the start of the heap, immediately after the MemControlBlock\n";
            } else {
                cout << " - Failed: address returned should have been " << sizeof(MemControlBlock) << " bytes after the start of the heap, but was ";
                
                cout << static_cast<size_t>(addrOne - heap.getStartOfHeap());
                cout << " bytes after it\n";
                cout << "  * heap.print() gives\n";
                heap.print();
            }
        }
        
        cout << "\nMaking second allocation request";
        cout.flush();
        auto addrTwo = heap.allocateMemoryWorstFit(bigRequest);
                
        if (addrTwo == nullptr) {
            cout << " - Success: returned nullptr\n";
        } else {
            cout << " - Failed: should have failed, returning nullptr, but instead returned an address ";                
            cout << static_cast<size_t>(addrTwo - heap.getStartOfHeap());
            cout << " bytes after the start of the heap\n";
            cout << "  * heap.print() gives\n";
            heap.print();            
        }
        
        cout << "--------------------------------------------------------------------------------------\n";
    }
    
    cout << "Making a new empty heap of size 1048576 for the remaining tests:\n\n";
    
    Heap heap(1048576);
    
    vector<char*> created(4);
    
    {
        cout << "Test 1: making space for a TestObject\n";
        
        char * addrOne = heap.allocateMemoryWorstFit(sizeof(TestObject));
        
        if (addrOne == nullptr) {
            cout << " - Failed: returned nullptr\n";
            cout << "  * heap.print() gives\n";
            heap.print();
        } else {
            char * shouldBe = heap.getStartOfHeap() + sizeof(MemControlBlock);
            if (shouldBe == addrOne) {
                cout << " - Passed: object created at the start of the heap, immediately after the MemControlBlock\n";
            } else {
                cout << " - Failed: address returned should have been " << sizeof(MemControlBlock) << " bytes after the start of the heap, but was ";
                
                cout << static_cast<size_t>(addrOne - heap.getStartOfHeap());
                cout << " bytes after it\n";
                cout << "  * heap.print() gives\n";
                heap.print();
            }
        }

        
        created[0] = addrOne;
    }
    
    for (int i = 1; i < 4; ++i) {
        cout << "\nTest " << i + 1 << ": making space for another TestObject\n";
        
        char * addrTwo = heap.allocateMemoryWorstFit(sizeof(TestObject));
        
        created[i] = addrTwo;
        
        if (addrTwo == nullptr) {
            cout << " - Failed: returned nullptr\n";
            cout << "  * heap.print() gives\n";
            heap.print();
        } else {
            char * shouldBe = created[i - 1] + sizeof(TestObject) + sizeof(MemControlBlock);
            if (shouldBe == addrTwo) {
                cout << " - Passed: object created after the MemControlBlock after test object number " << i << endl;
            } else {
                cout << " - Failed: address returned should have been " << sizeof(MemControlBlock) << " bytes after the start of the heap, but was ";
                
                cout << static_cast<size_t>(addrTwo - heap.getStartOfHeap());
                cout << " bytes after it\n";
                cout << "  * heap.print() gives\n";
                heap.print();
            }
        }
    }

    cout << "\nDeallocating the first object (this will crash if it was null)\n";    
    heap.deallocateMemory(created[0]);
    cout << "  * heap.print() gives\n";
    heap.print();
    cout << "\nDeallocating the second object (this will crash if it was null)\n";    
    heap.deallocateMemory(created[1]);
    cout << "  * heap.print() gives\n";
    heap.print();
    
    
    created.resize(6);
    
    {
        
        cout << "\nTest 5 - Creating a fifth object of size ";
        cout << sizeof(TestObject) << endl;
        cout << "(This would easily fit in the hole after freeing the first two allocations, but should still go in the large heap after object four, as that's a worse fit.)\n\n";
    
        char * addrOne = heap.allocateMemoryWorstFit(sizeof(TestObject));
        
        created[4] = addrOne;

        if (addrOne == nullptr) {
            cout << " - Failed: returned nullptr\n";
            cout << "  * heap.print() gives\n";
            heap.print();
        } else {
            char * shouldBe = created[3] + sizeof(TestObject) + sizeof(MemControlBlock);
            if (shouldBe == addrOne) {
                cout << " - Passed: object created in the large hole that was after the fourth object\n";
            } else {
                cout << " - Failed: address returned should have been " << sizeof(TestObject) + sizeof(MemControlBlock) << " bytes after the start of the fourth object, but was ";
              
                if (addrOne < created[3]) {
                    cout << static_cast<size_t>(created[3] - addrOne);
                    cout << " bytes before it\n";
                } else {
                    cout << static_cast<size_t>(addrOne - created[3]);
                    cout << " bytes after it\n";
                }
                
                cout << "  * heap.print() gives\n";
                heap.print();
            }
        }
        
    }
    
    {
        char * endOfHeap = heap.getStartOfHeap() + 1048576;
        size_t lastHoleSize = endOfHeap - created[4] - sizeof(TestObject) - sizeof(MemControlBlock);
        
        
        cout << "\nTest 6 - Creating a sixth object of size ";
        cout << lastHoleSize << endl;
        cout << "(This should be exactly the size of the last hole on the heap, after all previous allocations.)\n\n";
        
        char * addrOne = heap.allocateMemoryWorstFit(lastHoleSize);
        
        created[5] = addrOne;

        if (addrOne == nullptr) {
            cout << " - Failed: returned nullptr\n";
            cout << "  * heap.print() gives\n";
            heap.print();
        } else {
            char * shouldBe = created[4] + sizeof(TestObject) + sizeof(MemControlBlock);
            if (shouldBe == addrOne) {
                cout << " - Passed: object created in the large hole that was after the fifth object\n";
            } else {
                cout << " - Failed: address returned should have been " << sizeof(TestObject) + sizeof(MemControlBlock) << " bytes after the start of the fifth object, but was ";
              
                if (addrOne < created[3]) {
                    cout << static_cast<size_t>(created[4] - addrOne);
                    cout << " bytes before it\n";
                } else {
                    cout << static_cast<size_t>(addrOne - created[4]);
                    cout << " bytes after it\n";
                }
                
                cout << "  * heap.print() gives\n";
                heap.print();
            }
        }
        
    }

    {
        
        cout << "\nTest 7 - Creating one last object of size ";
        cout << sizeof(TestObject) << endl;
        cout << "(This should go in the first hole.)\n\n";
    
        char * addrOne = heap.allocateMemoryWorstFit(sizeof(TestObject));
        
        
        if (addrOne == nullptr) {
            cout << " - Failed: returned nullptr\n";
            cout << "  * heap.print() gives\n";
            heap.print();
        } else {
            char * shouldBe = heap.getStartOfHeap() + sizeof(MemControlBlock);
            if (shouldBe == addrOne) {
                cout << " - Passed: object created at the start of the first hole\n";
            } else {
                cout << " - Failed: address returned should have been " << sizeof(MemControlBlock) << " bytes after the start of the heap, but was ";
              
                
                cout << static_cast<size_t>(addrOne - heap.getStartOfHeap());
                cout << " bytes after it\n";
                
                cout << "  * heap.print() gives\n";
                heap.print();
            }
        }
        
    }
}
