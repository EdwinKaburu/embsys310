#include <assert.h>
#include "stack.h"

int main()
{
    
    
    // Val
    int val1, val2,val3, val4, val5;

    // Result
    int in_stack1;

    int test_val;


    // Test 1: Test Retrieval (Pop) of Empty Stack

    // Arrange

    // Initialize stack
    stack_init();


    val1 = 5;
    in_stack1 = 5;
    // Act
    in_stack1 = stack_pop(&val1);

    //Assert

    assert(-1 == in_stack1); // value returned from removing an empty stack
    assert(5 == val1);
    assert(1 == isempty()); // Method to check if stack is empty


    // Test 2: Push and Pop  values into and from Stack

    // Arrange
    val1 = 5;
    val2 =6;
    val3 =7;
    val4 = 8;
    val5 = 9;

    test_val = 0;

    // Act : LIFO

    stack_push(val1);
    stack_push(val2);
    stack_push(val3);
    stack_push(val4);
    stack_push(val5);

    // Assert

    assert(-1 == stack_push(10));

    //Method/function to check if stack is empty
    assert(1 == isfull());

    // Empty Stack and Confirm Popped Result
    assert(0 == stack_pop(&test_val));
    assert(5 == test_val);

    assert(0 == stack_pop(&test_val));
    assert(6 == test_val);

    assert(0 == stack_pop(&test_val));
    assert(7 == test_val);

    assert(0 == stack_pop(&test_val));
    assert(8 == test_val);

    assert(0 == stack_pop(&test_val));
    assert(9 == test_val);


    // Test 3 Just For Fun: Re-fill stack, then  Pop/remove the 3 Last Item and Push New Values
    // Test Will Fail
    // stack 1( 5,6,7,8,9 )  -- 3 times --> stack_pop(), ---- stack_push() --> (1,2,3,8,9)

    // Arrange
    test_val = 0;
    val1 = 5;
    val2 =6;
    val3 =7;
    val4 = 8;
    val5 = 9;


    // Act

    stack_push(val1);
    stack_push(val2);
    stack_push(val3);
    stack_push(val4);
    stack_push(val5);

    // Assert
    
    assert(0 == stack_pop(&test_val));
    assert(5 == test_val);

    assert(0 == stack_pop(&test_val));
    assert(6 == test_val);

    assert(0 == stack_pop(&test_val));
    assert(7 == test_val);

    //push 1
    assert(0 == stack_push(1));

    //push 2
    assert(0 == stack_push(2));

    //push 3
   assert(0 == stack_push(3));

    
  return 0;
}
