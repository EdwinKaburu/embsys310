#include "stack.h"

#define STACK_SIZE 5

// STACK DataStore
int Stack_Data[STACK_SIZE];

// Pointer Where To Insert Element
int *putPtr;

// Pointer to Last Item (LIFO)
int *getPtr;

// Number of Element
int counter;

//Initialize Pointer

void stack_init(void) {
    putPtr = Stack_Data;
    getPtr = Stack_Data;
    counter = 0;
}

int stack_push(int data) {

    // If Full Return -1
    if (isfull())
    {
        return  -1;
    }

    //Insert where putPtr is currently poiting to
    *putPtr = data;

    // Next Available Index
   // putPtr++;

   // No Increment past Storage Size Limit
    if(counter != (STACK_SIZE -1) )
    {
        putPtr++;
    }

    //Number of Elements
    counter++;

    return 0;
}

int stack_pop(int *data) {

    if(isempty())
    {
        return  -1;
    }

    // Get Data and Return it
    *data = *getPtr;

    // Reset the Current List Values
    *getPtr = 0;

   //Reset getPtr to the First Index
    if ((*(getPtr + 1)) == 0)
    {
        getPtr = Stack_Data;
    }
    else
    {
        getPtr = (getPtr + 1);
    }

    // Rest Put_PTR to first index, when full (circular stack)
    if (isfull())
    {
        putPtr = Stack_Data;
    }

    counter--;

    return 0;
}

int isempty(void) {
   
    // Return 1 when Empty

    if(counter == 0)
    {
        // True: Stack is Empty
        return  1;
    }

    //False : Stack is not Empty
    return 0;
}

int isfull(void)
{

    if(counter == STACK_SIZE)
    {
        // True if stack is Full
        return 1;
    }

    // False if stack is not Full
    return 0;
}









