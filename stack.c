#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "tree.h"
#include "queue.h"

extern HistoryNode* historyHead;
extern StackNode* addStack;

StackNode* Stack_createNode(const char* operation, TreeNode* node, TreeNode* parent) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (!newNode) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strcpy(newNode->operation, operation);
    newNode->node = node;
    newNode->parent = parent;
    newNode->next = NULL;
    return newNode;
}

void Stack_pushAdd(StackNode** head, const char* name, const char* type, const char* parentName, const char* operation) {
    StackNode* newNode = Stack_createNode(operation, NULL, NULL);
    strcpy(newNode->name, name);
    strcpy(newNode->type, type);
    strcpy(newNode->parentName, parentName);
    newNode->next = *head;
    *head = newNode;
}

void Stack_pushDelete(StackNode** head, TreeNode* node, TreeNode* parent, const char* operation) {
    StackNode* newNode = Stack_createNode(operation, node, parent);
    strcpy(newNode->name, node->name);
    strcpy(newNode->type, node->type);
    newNode->next = *head;
    *head = newNode;
}

void Stack_undoAdd(StackNode** addStack, StackNode** redoStack, QueueNode** queueHead, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*)) {
    if (!*addStack) {
        printf("No add operations to undo!\n");
        return;
    }
    StackNode* top = *addStack;
    *addStack = top->next;

    StackNode* redoNode = Stack_createNode(top->operation, NULL, NULL);
    strcpy(redoNode->name, top->name);
    strcpy(redoNode->type, top->type);
    strcpy(redoNode->parentName, top->parentName);
    redoNode->next = *redoStack;
    *redoStack = redoNode;

    QueueNode* current = *queueHead;
    QueueNode* prev = NULL;
    while (current) {
        if (strcmp(current->name, top->name) == 0 && strcmp(current->type, top->type) == 0 &&
            strcmp(current->parentName, top->parentName) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                *queueHead = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }

    char operation[200];
    snprintf(operation, sizeof(operation), "Undid add %s (%s) under %s", top->name, top->type, top->parentName);
    addToHistory(&historyHead, operation);
    free(top);
    printf("Undo add completed!\n");
}

void Stack_redoAdd(StackNode** redoStack, StackNode** addStack, QueueNode** queueHead, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*)) {
    if (!*redoStack) {
        printf("No operations to redo!\n");
        return;
    }
    StackNode* top = *redoStack;
    *redoStack = top->next;

    QueueNode* node = Queue_createNode(top->name, top->type, top->parentName);
    node->next = *queueHead;
    *queueHead = node;

    Stack_pushAdd(addStack, top->name, top->type, top->parentName, top->operation);

    char operation[200];
    snprintf(operation, sizeof(operation), "Redid add %s (%s) under %s", top->name, top->type, top->parentName);
    addToHistory(&historyHead, operation);
    free(top);
    printf("Redo add completed!\n");
}

void Stack_undoDelete(StackNode** deleteStack, StackNode** redoStack, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*)) {
    if (!*deleteStack) {
        printf("No delete operations to undo!\n");
        return;
    }
    StackNode* top = *deleteStack;
    *deleteStack = top->next;

    StackNode* redoNode = Stack_createNode(top->operation, top->node, top->parent);
    strcpy(redoNode->name, top->name);
    strcpy(redoNode->type, top->type);
    redoNode->next = *redoStack;
    *redoStack = redoNode;

    TreeNode* parent = top->parent;
    Tree_addNode(parent, top->node);

    char operation[200];
    snprintf(operation, sizeof(operation), "Undid delete %s (%s)", top->name, top->type);
    addToHistory(&historyHead, operation);
    free(top);
    printf("Undo delete completed!\n");
}

void Stack_free(StackNode* head) {
    while (head) {
        StackNode* temp = head;
        head = head->next;
        free(temp);
    }
}