#ifndef TREE_H
#define TREE_H

#include "cJSON.h"
#include "history.h"

typedef struct StackNode StackNode;
typedef struct QueueNode QueueNode;

typedef struct TreeNode {
    char name[100];
    char type[20];
    struct TreeNode** children;
    int childCount;
} TreeNode;

TreeNode* Tree_createNode(const char* name, const char* type);
TreeNode* Tree_parseJSON(cJSON* json);
void Tree_addNode(TreeNode* parent, TreeNode* newNode);
TreeNode* Tree_findNode(TreeNode* node, const char* name);
TreeNode* Tree_findParent(TreeNode* node, TreeNode* target); 

#endif