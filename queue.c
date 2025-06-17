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

int Queue_checkDuplicate(QueueNode* head, TreeNode* root, const char* name) {
    // Check queue
    QueueNode* current = head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return 1; // Duplikat ditemukan
        }
        current = current->next;
    }
    // Check tree
    if (Tree_findNode(root, name)) {
        return 1; // Duplikat ditemukan
    }
    return 0; // Tidak ada duplikat
}

void Queue_add(QueueNode** head, const char* name, const char* type, const char* parentName, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*), void (*pushAddStack)(StackNode**, const char*, const char*, const char*, const char*)) {
    // Check for duplicate
    if (Queue_checkDuplicate(*head, root, name)) {
        printf("Error: Entity with name %s already exists in queue or tree!\n", name);
        return;
    }

    char finalParentName[100];
    strcpy(finalParentName, parentName);

    // Special handling for RT
    if (strcmp(type, "rt") == 0) {
        TreeNode* kelurahan = Tree_findNode(root, parentName);
        if (!kelurahan || strcmp(kelurahan->type, "kelurahan/desa") != 0) {
            printf("Error: Parent %s is not a kelurahan/desa!\n", parentName);
            return;
        }
        printf("Available RWs under %s:\n", parentName);
        int rwCount = 0;
        for (int i = 0; i < kelurahan->childCount; i++) {
            if (strcmp(kelurahan->children[i]->type, "rw") == 0) {
                printf("%d. %s\n", ++rwCount, kelurahan->children[i]->name);
            }
        }
        if (rwCount == 0) {
            printf("No RWs found under %s!\n", parentName);
            return;
        }
        printf("Enter RW name: ");
        char rwName[100];
        scanf(" %[^\n]", rwName);
        TreeNode* rw = Tree_findNode(kelurahan, rwName);
        if (!rw || strcmp(rw->type, "rw") != 0) {
            printf("Error: Invalid RW name!\n");
            return;
        }
        strcpy(finalParentName, rwName);
    }

    QueueNode* node = Queue_createNode(name, type, finalParentName);
    node->next = *head;
    *head = node;
    
    char operation[200];
    snprintf(operation, sizeof(operation), "Added %s (%s) to Queue under %s", name, type, finalParentName);
    addToHistory(&historyHead, operation);
    pushAddStack(&addStack, name, type, finalParentName, operation);
    printf("Entity added to Queue!\n");
}

void Queue_processToTree(QueueNode** head, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*)) {
    if (!*head) {
        printf("Queue is empty, nothing to process!\n");
        return;
    }

    // Display queue contents with numbering
    QueueNode* current = *head;
    int index = 1;
    printf("Queue contents:\n");
    while (current) {
        printf("%d. Name: %s, Type: %s, Parent: %s\n", index++, current->name, current->type, current->parentName);
        current = current->next;
    }

    // Ask user to select an entity to process
    int choice;
    printf("Enter the number of the entity to process (1 to %d, or 0 to cancel): ", index - 1);
    scanf("%d", &choice);

    if (choice == 0) {
        printf("Process cancelled.\n");
        return;
    }

    if (choice < 1 || choice >= index) {
        printf("Invalid choice!\n");
        return;
    }

    // Find the selected node
    current = *head;
    QueueNode* prev = NULL;
    index = 1;
    while (current && index < choice) {
        prev = current;
        current = current->next;
        index++;
    }

    if (!current) {
        printf("Entity not found!\n");
        return;
    }

    // Process the selected node to tree
    TreeNode* parent = Tree_findNode(root, current->parentName);
    if (!parent) {
        printf("Parent %s not found in tree!\n", current->parentName);
        return;
    }

    TreeNode* newNode = Tree_createNode(current->name, current->type);
    Tree_addNode(parent, newNode);

    char operation[200];
    snprintf(operation, sizeof(operation), "Processed %s (%s) from Queue to Tree under %s", current->name, current->type, current->parentName);
    addToHistory(&historyHead, operation);

    // Remove from queue
    if (prev) {
        prev->next = current->next;
    } else {
        *head = current->next;
    }
    free(current);

    printf("Entity processed to tree successfully!\n");
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