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
    
    // Exit or return -1, if stack is full
    if (isfull())
    {
        return  -1;
    }
    
    // Insert data where putPtr is currently is
    *putPtr = data;
    
    // This is the Last Item inserted (GetPtr always points to last item added, From putPtr)
    getPtr = putPtr;
    
    // Increment Number of Items in Stack
    counter++;
    
    // No Incrementation If Stack Is Full
    if (isfull() != 1)
    {
        putPtr++;
    }
    
    return  0;
}

int stack_pop(int *data) {
    
    // Exit or return -1, is stack is Empty
    if (isempty())
    {
        return -1;
    }
    
    // Get Data And Return It
    // getPtr will always point to the Last (LIFO)
    *data = *getPtr;
    
    //Reset To Zero
    *getPtr =0;
    
    // Decrement
    getPtr--;
    
    // Decrement Counter
    counter--;
    
    // putPtr points to last Value popped , so that new values can be added.
    if(counter != (STACK_SIZE -1) )
    {
        putPtr--;
    }
    
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









