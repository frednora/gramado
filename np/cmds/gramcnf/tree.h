// tree.c 
// bst - Binary Search Tree
// Evaluation an expression.
// Document created by Fred Nora.
// Credits: I don't know who is the original author.


#ifndef __TREE_H
#define __TREE_H    1


#define DT_INVALID   0
#define DT_OPERATOR  1000
#define DT_DIGIT     2000
// ...

// Node
struct node_d 
{ 

//  0000 = ignore
//  1000 = operator
//  2000 = digits
//  ...
    int _datatype;  // Type of data
    int _data;   // Data

    struct node_d *left;
    struct node_d *right;
}; 


// Stack
struct stack_d
{
    int top;
    int items[32];
};


// This function gets the expression from stdin?
unsigned long tree_eval(void);

#endif    

