// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>
#include <algorithm>

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr}
            {}

            // Description: Allows access to the element at that Node's
            // position.  There are two versions, getElt() and a dereference
            // operator, use whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data
            // members of this Node class from within the PairingPQ class.
            // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
            // function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
    }; // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
            rootptr = nullptr;
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
            rootptr = nullptr;
            while(start != end){
                push(*start);
                start++;
            }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare } {
            rootptr = nullptr;
            std::deque<Node *> dq;
            dq.push_back(other.rootptr);
            while(!dq.empty()){
                Node* cur = dq.front();
                if(cur->child != nullptr){
                    dq.push_back(cur->child);
                }
                if(cur->sibling != nullptr){
                    dq.push_back(cur->sibling);
                }
                push(cur->elt);
                dq.pop_front();
            }
            numNodes = other.size();
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid pairing heap.
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        PairingPQ temp(rhs);
    
        std::swap(numNodes, temp.numNodes);
        std::swap(rootptr, temp.rootptr);
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        // lecture.
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        std::deque<Node *> dq;
        Node* cur;
        dq.push_back(rootptr);
        if(!empty()){
            while(!dq.empty()){
                cur = dq.front();
                dq.pop_front();
                //this->pop();
                if(cur->child != nullptr){
                    dq.push_back(cur->child);
                    cur->child = nullptr;
                }
                if(cur->sibling != nullptr){
                    dq.push_back(cur->sibling);
                    cur->sibling = nullptr;
                }
                delete cur;
            }
        }
        else
            delete rootptr;
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        std::deque<Node *> dq;
        Node* cur;
        dq.push_back(rootptr);
        rootptr = nullptr;
        while(!dq.empty()){
            cur = dq.front();
            dq.pop_front();
            if(cur->child != nullptr){
                dq.push_back(cur->child);
                cur->child = nullptr;
            }
            if(cur->sibling != nullptr){
                dq.push_back(cur->sibling);
                cur->sibling = nullptr;
            }
            cur->parent = nullptr;
            rootptr = meld(rootptr, cur);
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        std::deque<Node *> dq;
        Node* temp = rootptr->child;
        delete rootptr;
        if(temp == nullptr){
            rootptr = nullptr;
        }
        else if(temp->sibling == nullptr){//only one node to consider
            rootptr = temp;
            rootptr->parent = nullptr;
        }
        else{
            dq.push_back(temp);
            while(temp->sibling != nullptr){
                dq.push_back(temp->sibling);
                temp = temp->sibling;
            }
            while(dq.size() > 1){
                Node* cur1 = dq.front();
                dq.pop_front();
                Node* cur2 = dq.front();
                dq.pop_front();
                cur1->sibling = nullptr;
                cur2->sibling = nullptr;
                dq.push_back(meld(cur1, cur2));
            }
            //at this point only 1 in deque
            rootptr = dq.front();
            rootptr->parent = nullptr;
        }
        numNodes--;
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return rootptr->getElt();
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return numNodes;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        if(numNodes == 0)
            return true;
        return false;
    } // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {//always increases priority of node
        node->elt = new_value;
        if(node == rootptr)
            return;
        if(this->compare(node->parent->elt, node->elt)){//parent is less priority than node
            Node* temp = node->parent->child;
            if(temp == node && node->sibling == nullptr){//only child
                node->parent->child = nullptr;
                node->parent = nullptr;
                rootptr = meld(rootptr, node);
            }
            else if(temp == node){//left most child 
                node->parent->child = node->sibling;
                node->sibling = nullptr;
                node->parent = nullptr;
                rootptr = meld(rootptr, node);
            }
            else{//middle or right most child
                if(temp->sibling != nullptr){
                    while(temp->sibling != node)
                        temp = temp->sibling;//
                }
                temp->sibling = node->sibling;
                node->sibling = nullptr;
                node->parent = nullptr;
                rootptr = meld(rootptr, node);
                //rootptr->parent = nullptr;
            }
        }
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node* addNode(const TYPE &val) {
        Node* temp = new Node(val);
        rootptr = meld(rootptr, temp);
        numNodes++;
        return temp;
    } // addNode()


private:
    Node* rootptr;
    size_t numNodes = 0;

    Node * meld(Node* tree1, Node* tree2){
        if(!tree1)
            return tree2;
        else if(!tree2)
            return tree1;
        if(this->compare(tree1->getElt(), tree2->getElt())){//tree2 has higher priority
            Node* temp = tree2->child;
            tree2->child = tree1;
            tree1->parent = tree2;
            tree1->sibling = temp;
            return tree2;
        }
        //keeps tree1 as root
        Node* temp = tree1->child;
        tree1->child = tree2;
        tree2->parent = tree1;
        tree2->sibling = temp;
        return tree1;

    }

    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
};


#endif // PAIRINGPQ_H
