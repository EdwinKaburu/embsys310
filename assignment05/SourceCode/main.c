#include "swap.h"

// Arguments/Parameters : 
// Note to self(11/16/20) A Pointer pointing to a pointer(we are swapping pointers)

void swap_ptr(int **x_ptr, int **y_ptr)
{
    //A temp_pointer pointing ( to the x_ptr pointer)
    int *temp_ptr = *x_ptr;
    
    // x_ptr  pointing to the y_ptr
    *x_ptr = *y_ptr;
    
    //y_ptr pointing to the x_ptr
    
    *y_ptr = temp_ptr;
}

int main()
{
    int val = 100000;
    
    int val1 = 200000;
    
    // Pointer to Memory/Address of Val
    int* xPtr = &val;
    
    // Pointer to Memory/Address of Val1
    int* yPtr = &val1;
    
    
    // Perform Swap of Pointers
    // End Result should be vice versa 
    swap_ptr(&xPtr, &yPtr);
    
	// int *temp = xPtr;

    
    return 0;
}
