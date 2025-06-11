#ifndef QUEUE_H
#define QUEUE_H

#include "history.h"


typedef struct StackNode StackNode;
typedef struct TreeNode TreeNode;

typedef struct QueueNode {
    char name[100];
    char type[20];
    char parentName[100];
    struct QueueNode* next;
} QueueNode;

// fungsi prototypes
QueueNode* Queue_createNode(const char* name, const char* type, const char* parentName);
void Queue_add(QueueNode** head, const char* name, const char* type, const char* parentName, void (*addToHistory)(HistoryNode**, const char*), void (*pushAddStack)(StackNode**, const char*));
void Queue_processToTree(QueueNode** head, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*));
void Queue_edit(QueueNode* head, const char* name, void (*addToHistory)(HistoryNode**, const char*));
void Queue_delete(QueueNode** head, const char* name, void (*addToHistory)(HistoryNode**, const char*));
void Queue_display(QueueNode* head);
void Queue_free(QueueNode* head);

#endif