#include <os/OSError.h>

#include "pp/linkedlist.h"

void testLinkedList() {
    linkedlist<int>* ll = new linkedlist<int>();

    int ints[4] = {1, 2, 3, 0}; 
    int* intPtr = ints;

    ll->append(*intPtr++);
    ll->append(*intPtr++);
    ll->append(*intPtr++);
    ll->prepend(*intPtr++);

    LinkedlistIterator<int> itr = LinkedlistIterator<int>(*ll);

    OSReport("Linked list test 1:\n");
    int* p;
    while((p = itr.next()) != 0){
        OSReport(" - %d\n", *p);
    }


    itr = LinkedlistIterator<int>(*ll);
    itr.next(); // 0
    itr.next(); // 1
    itr.deleteHere(); // goodbye 1

    int* newInt = new int();
    *newInt = 99;
    itr.insertHere(*newInt); // hello 99
    p = itr.next();

    itr = LinkedlistIterator<int>(*ll);

    while((p = itr.next()) != 0){
        OSReport(" - %d\n", *p);
    }


    delete ll;
    delete newInt;
}