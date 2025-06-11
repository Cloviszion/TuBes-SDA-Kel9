#ifndef HISTORY_H
#define HISTORY_H

typedef struct HistoryNode {
    char operation[200];
    struct HistoryNode* next;
} HistoryNode;

// Function prototypes
HistoryNode* History_createNode(const char* operation);
void History_add(HistoryNode** head, const char* operation);
void History_display(HistoryNode* head);
void History_free(HistoryNode* head);

#endif