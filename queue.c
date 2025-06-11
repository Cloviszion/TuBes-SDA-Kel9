#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "tree.h"

extern HistoryNode* historyHead;
extern StackNode* addStack;

QueueNode* Queue_createNode(const char* name, const char* type, const char* parentName) {
    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!node) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strcpy(node->name, name);
    strcpy(node->type, type);
    strcpy(node->parentName, parentName);
    node->next = NULL;
    return node;
}

void Queue_add(QueueNode** head, const char* name, const char* type, const char* parentName, void (*addToHistory)(HistoryNode**, const char*), void (*pushAddStack)(StackNode**, const char*)) {
    QueueNode* node = Queue_createNode(name, type, parentName);
    node->next = *head;
    *head = node;
    
    char operation[200];
    snprintf(operation, sizeof(operation), "Added %s (%s) to Queue under %s", name, type, parentName);
    addToHistory(&historyHead, operation);
    pushAddStack(&addStack, operation);
    printf("Entity added to Queue!\n");
}

void Queue_processToTree(QueueNode** head, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*)) {
    QueueNode* current = *head;
    QueueNode* prev = NULL;
    
    while (current) {
        TreeNode* parent = Tree_findNode(root, current->parentName);
        if (!parent) {
            printf("Parent %s not found for %s!\n", current->parentName, current->name);
            prev = current;
            current = current->next;
            continue;
        }
        
        TreeNode* newNode = Tree_createNode(current->name, current->type);
        Tree_addNode(parent, newNode);
        
        char operation[200];
        snprintf(operation, sizeof(operation), "Processed %s (%s) from Queue to Tree under %s", current->name, current->type, current->parentName);
        addToHistory(&historyHead, operation);
        
        if (prev) {
            prev->next = current->next;
        } else {
            *head = current->next;
        }
        QueueNode* temp = current;
        current = current->next;
        free(temp);
    }
    printf("Queue processed to Tree!\n");
}

void Queue_edit(QueueNode* head, const char* name, void (*addToHistory)(HistoryNode**, const char*)) {
    QueueNode* current = head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            printf("Enter new name: ");
            scanf(" %[^\n]", current->name);
            printf("Enter new type: ");
            scanf(" %[^\n]", current->type);
            printf("Enter new parent name: ");
            scanf(" %[^\n]", current->parentName);
            
            char operation[200];
            snprintf(operation, sizeof(operation), "Edited %s in Queue", name);
            addToHistory(&historyHead, operation);
            printf("Queue data edited!\n");
            return;
        }
        current = current->next;
    }
    printf("Entity not found in Queue!\n");
}

void Queue_delete(QueueNode** head, const char* name, void (*addToHistory)(HistoryNode**, const char*)) {
    QueueNode* current = *head;
    QueueNode* prev = NULL;
    
    while (current) {
        if (strcmp(current->name, name) == 0) {
            char operation[200];
            snprintf(operation, sizeof(operation), "Deleted %s from Queue", name);
            addToHistory(&historyHead, operation);
            
            if (prev) {
                prev->next = current->next;
            } else {
                *head = current->next;
            }
            free(current);
            printf("Entity deleted from Queue!\n");
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("Entity not found in Queue!\n");
}

void Queue_display(QueueNode* head) {
    if (!head) {
        printf("Queue is empty.\n");
        return;
    }
    printf("Queue contents:\n");
    QueueNode* current = head;
    while (current) {
        printf("Name: %s, Type: %s, Parent: %s\n", current->name, current->type, current->parentName);
        current = current->next;
    }
}

void Queue_free(QueueNode* head) {
    while (head) {
        QueueNode* temp = head;
        head = head->next;
        free(temp);
    }
}