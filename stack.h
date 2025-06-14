#ifndef STACK_H
#define STACK_H

typedef struct TreeNode TreeNode;
typedef struct QueueNode QueueNode;
typedef struct HistoryNode HistoryNode;

typedef struct StackNode {
    char name[100];
    char type[20];
    char parentName[100];
    char operation[200];
    TreeNode* node; 
    TreeNode* parent; 
    struct StackNode* next;
} StackNode;

StackNode* Stack_createNode(const char* operation, TreeNode* node, TreeNode* parent);
void Stack_pushAdd(StackNode** head, const char* name, const char* type, const char* parentName, const char* operation);
void Stack_pushDelete(StackNode** head, TreeNode* node, TreeNode* parent, const char* operation);
void Stack_undoAdd(StackNode** addStack, StackNode** redoStack, QueueNode** queueHead, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*));
void Stack_redoAdd(StackNode** redoStack, StackNode** addStack, QueueNode** queueHead, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*));
void Stack_undoDelete(StackNode** deleteStack, StackNode** redoStack, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*));
void Stack_free(StackNode* head);

#endif