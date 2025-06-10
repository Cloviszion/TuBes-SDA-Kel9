#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "queue.h"
#include "tree.h"

extern HistoryNode* historyHead;
extern QueueNode* queueHead;
extern TreeNode* root;
extern StackNode* addStack;
extern StackNode* redoStack;

StackNode* Stack_createNode(const char* operation, TreeNode* node) {
    StackNode* stackNode = (StackNode*)malloc(sizeof(StackNode));
    if (!stackNode) {
        printf("Alokasi memori gagal!\n");
        exit(1);
    }
    strcpy(stackNode->operation, operation);
    stackNode->node = node;
    stackNode->parent = NULL;
    stackNode->queueNode = NULL;
    stackNode->next = NULL;
    return stackNode;
}

void Stack_pushAdd(StackNode** head, const char* operation) {
    StackNode* stackNode = Stack_createNode(operation, NULL);
    stackNode->next = *head;
    *head = stackNode;
}

void Stack_pushDelete(StackNode** head, TreeNode* node, TreeNode* parent, const char* operation) {
    StackNode* stackNode = Stack_createNode(operation, node);
    stackNode->parent = parent;
    stackNode->next = *head;
    *head = stackNode;
}

void Stack_undoAdd(StackNode** addStack, QueueNode** queueHead, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*)) {
    if (!*addStack) {
        printf("Tidak ada operasi tambah untuk dibatalkan!\n");
        return;
    }
    
    StackNode* stackNode = *addStack;
    *addStack = (*addStack)->next;
    
    char operation[200];
    snprintf(operation, sizeof(operation), "Batalkan %s", stackNode->operation);
    addToHistory(&historyHead, operation);
    
    char* name = strstr(stackNode->operation, "Tambah ") + 7;
    char* end = strchr(name, ' ');
    if (end) *end = '\0';
    
    QueueNode* current = *queueHead;
    QueueNode* prev = NULL;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                *queueHead = current->next;
            }
            stackNode->queueNode = current;
            stackNode->next = redoStack;
            redoStack = stackNode;
            printf("Pembatalan tambah dari queue berhasil!\n");
            return;
        }
        prev = current;
        current = current->next;
    }

    TreeNode* target = Tree_findNode(root, name);
    if (target) {
        TreeNode* parent = Tree_findParent(root, target);
        if (parent) {
            stackNode->node = target;
            stackNode->parent = parent;
            for (int i = 0; i < parent->childCount; i++) {
                if (parent->children[i] == target) {
                    for (int j = i; j < parent->childCount - 1; j++) {
                        parent->children[j] = parent->children[j + 1];
                    }
                    parent->childCount--;
                    parent->children = (TreeNode**)realloc(parent->children, parent->childCount * sizeof(TreeNode*));
                    if (parent->childCount == 0) {
                        free(parent->children);
                        parent->children = NULL;
                    }
                    stackNode->next = redoStack;
                    redoStack = stackNode;
                    printf("Pembatalan tambah dari pohon berhasil!\n");
                    return;
                }
            }
        }
    }
    
    stackNode->next = redoStack;
    redoStack = stackNode;
    printf("Node tidak ditemukan untuk pembatalan!\n");
}

void Stack_redoAdd(StackNode** redoStack, QueueNode** queueHead, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*)) {
    if (!*redoStack) {
        printf("Tidak ada operasi untuk diulang!\n");
        return;
    }
    
    StackNode* stackNode = *redoStack;
    *redoStack = (*redoStack)->next;
    
    char operation[200];
    snprintf(operation, sizeof(operation), "Ulang %s", stackNode->operation);
    addToHistory(&historyHead, operation);
    
    if (stackNode->queueNode) {
        QueueNode* node = stackNode->queueNode;
        node->next = *queueHead;
        *queueHead = node;
        stackNode->queueNode = NULL;
        stackNode->next = addStack;
        addStack = stackNode;
        printf("Pengulangan tambah ke queue berhasil!\n");
        return;
    }

    if (stackNode->node && stackNode->parent) {
        Tree_addNode(stackNode->parent, stackNode->node);
        stackNode->node = NULL;
        stackNode->parent = NULL;
        stackNode->next = addStack;
        addStack = stackNode;
        printf("Pengulangan tambah ke pohon berhasil!\n");
        return;
    }
    
    stackNode->next = addStack;
    addStack = stackNode;
    printf("Gagal mengulang tambah!\n");
}

void Stack_undoDelete(StackNode** deleteStack, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*)) {
    if (!*deleteStack) {
        printf("Tidak ada operasi hapus untuk dibatalkan!\n");
        return;
    }
    
    StackNode* stackNode = *deleteStack;
    *deleteStack = (*deleteStack)->next; 
    
    char operation[200];
    snprintf(operation, sizeof(operation), "Batalkan %s", stackNode->operation);
    addToHistory(&historyHead, operation);
    
    TreeNode* node = stackNode->node;
    TreeNode* parent = stackNode->parent;
    if (node && parent && Tree_findNode(root, parent->name)) {
        Tree_addNode(parent, node);
        stackNode->node = NULL;
        stackNode->parent = NULL;
        free(stackNode);
        printf("Pembatalan hapus berhasil!\n");
        return;
    }
    
    free(stackNode);
    printf("Gagal membatalkan hapus!\n");
}

void Stack_free(StackNode* head) {
    while (head) {
        StackNode* temp = head;
        head = head->next;
        if (temp->queueNode) free(temp->queueNode);
        if (temp->node) Tree_free(temp->node);
        free(temp);
    }
}