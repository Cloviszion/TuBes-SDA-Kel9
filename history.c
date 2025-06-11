#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"

HistoryNode* History_createNode(const char* operation) {
    HistoryNode* node = (HistoryNode*)malloc(sizeof(HistoryNode));
    if (!node) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strcpy(node->operation, operation);
    node->next = NULL;
    return node;
}

void History_add(HistoryNode** head, const char* operation) {
    HistoryNode* node = History_createNode(operation);
    node->next = *head;
    *head = node;
}

void History_display(HistoryNode* head) {
    if (!head) {
        printf("No operations recorded.\n");
        return;
    }
    printf("Operation History:\n");
    while (head) {
        printf("%s\n", head->operation);
        head = head->next;
    }
}

void History_free(HistoryNode* head) {
    while (head) {
        HistoryNode* temp = head;
        head = head->next;
        free(temp);
    }
}