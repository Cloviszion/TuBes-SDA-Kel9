#ifndef STACK_H
#define STACK_H

#include "history.h"

typedef struct QueueNode QueueNode;
typedef struct TreeNode TreeNode;

typedef struct StackNode {
    TreeNode* node; 
    char operation[200]; 
    struct StackNode* next;
} StackNode;

StackNode* Stack_createNode(const char* operation, TreeNode* node);
void Stack_pushAdd(StackNode** head, const char* operation);
void Stack_pushDelete(StackNode** head, TreeNode* node, const char* operation);
void Stack_undoAdd(StackNode** addStack, QueueNode** queueHead, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*));
void Stack_undoDelete(StackNode** deleteStack, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*));
void Stack_free(StackNode* head);

#endif