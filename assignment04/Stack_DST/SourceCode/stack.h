#ifndef __STACK_H__
#define __STACK_H__

//function prototype
void stack_init(void);

int stack_push(int data);

int stack_pop(int *data);

int isempty(void);

int isfull(void);


#endif