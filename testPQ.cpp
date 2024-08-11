// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

/*
 * Compile this test against your .h files to make sure they compile. We
 * suggest adding to this file or creating your own test cases to test your
 * priority queue implementations more thoroughly. If you do not call a
 * function from here, that template is not instantiated and that function is
 * NOT compiled! So for instance, if you don't add code here to call
 * updatePriorities, that function could later cause compiler errors that you
 * don't even know about.
 *
 * Our makefile will build an executable named testPQ if you type 'make
 * testPQ' or 'make alltests' (without the quotes). This will be a debug
 * executable.
 *
 * Notice that testPairing tests the range-based constructor but main and
 * testPriorityQueue do not. Make sure to test the range-based constructor
 * for other PQ types, and also test the PairingPQ-specific member functions.
 *
 * This is NOT a complete test of your priority queues. You have to add code
 * to do more testing!
 *
 * You do not have to submit this file, but it won't cause problems if you
 * do.
 */

#include <cassert>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "BinaryPQ.h"
#include "Eecs281PQ.h"
#include "PairingPQ.h"
#include "SortedPQ.h"
#include "UnorderedPQ.h"


// A type for representing priority queue types at runtime
enum class PQType {
    Unordered,
    Sorted,
    Binary,
    Pairing,
};

// These can be pretty-printed :)
std::ostream& operator<<(std::ostream& ost, PQType pqType) {
    switch (pqType) {
    case PQType::Unordered:
        return ost << "Unordered";
    case PQType::Sorted:
        return ost << "Sorted";
    case PQType::Binary:
        return ost << "Binary";
    case PQType::Pairing:
        return ost << "Pairing";
    }

    return ost << "Unknown PQType";
}


// Compares two int const* on the integers they point to
struct IntPtrComp {
    bool operator()(const int *a, const int *b) const { return *a < *b; }
};


// Test the primitive operations on a priority queue:
// constructor, push, pop, top, size, empty.
template <template <typename...> typename PQ>
void testPrimitiveOperations() {
    std::cout << "Testing primitive priority queue operations..." << std::endl;

    PQ<int> pq {};
    Eecs281PQ<int>& eecsPQ = pq;

    eecsPQ.push(3);
    eecsPQ.push(4);
    assert(eecsPQ.size() == 2);
    assert(eecsPQ.top() == 4);

    eecsPQ.pop();
    assert(eecsPQ.size() == 1);
    assert(eecsPQ.top() == 3);
    assert(not eecsPQ.empty());

    eecsPQ.pop();
    assert(eecsPQ.size() == 0);
    assert(eecsPQ.empty());

    eecsPQ.push(5);
    eecsPQ.push(6);
    eecsPQ.push(8);
    eecsPQ.push(2);
    eecsPQ.pop();
    eecsPQ.push(10);
    eecsPQ.pop();
    eecsPQ.push(3);
    eecsPQ.push(9);
    
    assert(eecsPQ.size() == 5);
    assert(eecsPQ.top() == 9);

    std::cout << "testPrimitiveOperations succeeded!" << std::endl;
}


// Test that the priority queue uses its comparator properly. HiddenData
// can't be compared with operator<, so we use HiddenDataComp{} instead.
template <template <typename...> typename PQ>
void testHiddenData() {
    struct HiddenData {
        int data;
    };

    struct HiddenDataComp {
        bool operator()(const HiddenData &a, const HiddenData &b) const {
            // TODO: Finish this comparator
            (void)a;  // Delete this line when you finish this function
            (void)b;  // Delete this line when you finish this function
            return false;
        }
    };

    std::cout << "Testing with hidden data..." << std::endl;

    // TODO: Add code here to actually test with the HiddenData type.
    // Consider writing this code in the style of testPrimitiveOperations
    // above.

    std::cout << "testHiddenData succeeded!" << std::endl;
}


// Test the last public member function of Eecs281PQ, updatePriorities
template <template <typename...> typename PQ>
void testUpdatePriorities() {
    std::vector<int> data {
        1,
        5,
    };

    PQ<const int*, IntPtrComp> pq {};
    Eecs281PQ<const int*, IntPtrComp>& eecsPQ = pq;

    // NOTE: If you add more data to the vector, don't push the pointers
    //   until AFTER the vector stops changing size! Think about why.
    for (auto& datum : data) {
        eecsPQ.push(&datum);
    }

    // Change some element in data (which is pointed to by an element in pq).
    // This new value should be higher than any other so its address will
    // wind qt the top adter updatePriorities.
    auto& datum = data[0];
    datum = 10;
    eecsPQ.updatePriorities();
    assert(*eecsPQ.top() == 10);
    assert(eecsPQ.top() == &datum);

    
    std::vector<int> data2 { 
        26,
        28, 
        38,
        22,
        64,
        54,
        79
    };

    PQ<const int*, IntPtrComp> pq2 {};
    Eecs281PQ<const int*, IntPtrComp>& eecsPQ2 = pq2;

    for (auto& datum : data2) {
        eecsPQ2.push(&datum);
    }

    auto& datum2 = data2[2];
    datum2 = 83;
    eecsPQ2.updatePriorities();
    assert(*eecsPQ2.top() == 83);
    assert(eecsPQ2.top() == &datum2);
    
    // TODO: Add more testing here as you see fit.
}


// Test the pairing heap's range-based constructor, copy constructor,
// copy-assignment operator, and destructor
// TODO: Test other operations specific to this PQ type.
void testPairing() {
    std::cout << "Testing Pairing Heap separately..." << std::endl;

    {
        const std::vector<int> vec {
            1,
            0,
        };

        std::cout << "Calling constructors" << std::endl;

        // Range-based constructor
        PairingPQ<int> pairing1 { vec.cbegin(), vec.cend() };
        // Copy constructor
        PairingPQ<int> pairing2 { pairing1 };
        // Copy-assignment operator
        PairingPQ<int> pairing3 {};

        pairing3 = pairing2;

        std::cout << "p1 address " << &pairing1 << '\n';
        std::cout << "p2 address " << &pairing2 << '\n';
        std::cout << "p3 address " << &pairing3 << '\n';

        // A reference to a PairingPQ<T> is a reference to an Eecs281PQ<T>.
        // Yay for polymorphism! We can therefore write:
        Eecs281PQ<int>& pq1 = pairing1;
        Eecs281PQ<int>& pq2 = pairing2;
        Eecs281PQ<int>& pq3 = pairing3;

        pq1.push(3);
        pq2.pop();
        assert(pq1.size() == 3);
        assert(not pq1.empty());
        assert(pq1.top() == 3);
        pq2.push(pq3.top());
        assert(pq2.top() == pq3.top());

        std::cout << "Basic tests done." << std::endl;

        // TODO: Add more code to test addNode, updateElt, etc.

        //addNode alr tested?
        //Node* nNode = pq1.addNode(4);

        //updateElt
        PairingPQ<int> pairing4 {};

        std::vector<int> data3 {
            1
        };

        for (auto& datum : data3) {
            pairing4.push(datum);
        }

        auto* datum3 = pairing4.addNode(5);
        std::cout << "memory of datum3: " << &datum3 << "\n";
        pairing4.updateElt(datum3, 7);
        assert(pairing4.top() == 7);
        assert(pairing4.top() == **datum3);

        auto* datum4 = pairing4.addNode(4);
        std::cout << "memory of datum4: " << &datum4 << "\n";
        pairing4.updateElt(datum4, 9);
        assert(pairing4.top() == 9);
        assert(pairing4.top() == **datum4);

        PairingPQ<int>::Node* datum5 = pairing4.addNode(5);
        std::cout << "memory of datum5: " << &datum5 << "\n";
        pairing4.updateElt(datum5, 4);
        pairing4.updateElt(datum3, 6);

        assert(pairing4.top() == **datum4);
        assert(pairing4.top() == 9);

        pairing4.updateElt(datum3, 12);
        
        assert(pairing4.top() == 12);
        assert(pairing4.top() == **datum3);

        const auto& datum6 = pairing4.addNode(3);
        std::cout << "memory of datum6: " << &datum6 << "\n";
        pairing4.updateElt(datum6, 5);
        pairing4.updateElt(datum4, 13);

        assert(pairing4.top() == 13);
        assert(pairing4.top() == **datum4);

        const auto& datum7 = pairing4.addNode(3);
        std::cout << "memory of datum7: " << &datum7 << "\n";
        pairing4.updateElt(datum7, 6);
        pairing4.updateElt(datum5, 14);

        assert(pairing4.top() == 14);
        assert(pairing4.top() == **datum5);


        PairingPQ<int> pairing5 {};
        const auto& datum8 = pairing5.addNode(6);
        std::cout << "memory of datum8: " << &datum8 << "\n";
        const auto& datum9 = pairing5.addNode(4);
        std::cout << "memory of datum9: " << &datum9 << "\n";

        pairing5.updateElt(datum8, 7);
        pairing5.updateElt(datum9, 10);

        assert(pairing5.top() == 10);
        assert(pairing5.top() == **datum9);

        std::cout << "test updateElt complete!" << std::endl;

        std::cout << "memory of datum3: " << &datum3 << "\n";
        std::cout << "memory of datum4: " << &datum4 << "\n";
        std::cout << "memory of datum5: " << &datum5 << "\n";
        std::cout << "memory of datum6: " << &datum6 << "\n";
        std::cout << "memory of datum7: " << &datum7 << "\n";
        std::cout << "memory of datum8: " << &datum8 << "\n";
        std::cout << "memory of datum9: " << &datum9 << "\n";

        std:: cout << "test addNodes complete!" << std:: endl;

        // That { above creates a scope, and our pairing heaps will fall out
        // of scope at the matching } below.
        std::cout << "Calling destructors" << std::endl;
    }

    std::cout << "testPairing succeeded!" << std::endl;
}


// Run all tests for a particular PQ type.
template <template <typename...> typename PQ>
void testPriorityQueue() {
    testPrimitiveOperations<PQ>();
    testHiddenData<PQ>();
    testUpdatePriorities<PQ>();
}

// PairingPQ has some extra behavior we need to test in updateElement.
// This template specialization handles that without changing the nice
// uniform interface of testPriorityQueue.
template <>
void testPriorityQueue<PairingPQ>() {
    testPrimitiveOperations<PairingPQ>();
    testHiddenData<PairingPQ>();
    testUpdatePriorities<PairingPQ>();
    testPairing();
}


int main() {
    const std::vector<PQType> types {
        PQType::Unordered,
        PQType::Sorted,
        PQType::Binary,
        PQType::Pairing,
    };

    std::cout << "PQ tester" << std::endl << std::endl;
    int idx { 0 };
    for (const auto &type : types) {
        std::cout << "  " << idx++ << ") " << type << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Select one: ";
    uint32_t choice {};
    std::cin >> choice;
    const PQType pqType = types.at(choice);

    std::cout << "Testing the " << pqType << " PQ...";

    // TODO: Add more cases to test other priority queue types.
    switch (pqType) {
    case PQType::Unordered:
        testPriorityQueue<UnorderedPQ>();
        break;
    case PQType::Sorted:
        testPriorityQueue<SortedPQ>();
        break;
    case PQType::Binary:
        testPriorityQueue<BinaryPQ>();
        break;
    case PQType::Pairing:
        testPriorityQueue<PairingPQ>();
        break;
    default:
        std::cout << "Unrecognized PQ type " << pqType << " in main.\n"
                  << "You must add tests for all PQ types." << std::endl;
        return 1;
    }

    std::cout << "All tests succeeded!" << std::endl;

    return 0;
}
