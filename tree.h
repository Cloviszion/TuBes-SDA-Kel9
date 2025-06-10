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
void Tree_deleteNode(TreeNode** root, const char* name, void (*addToHistory)(HistoryNode**, const char*), void (*pushDeleteStack)(StackNode**, TreeNode*, TreeNode*, const char*));
void Tree_editNode(TreeNode* node, const char* name, const char* newName, const char* newType, void (*addToHistory)(HistoryNode**, const char*));
void Tree_searchNode(TreeNode* node, const char* name);
void Tree_displayPreorder(TreeNode* node, int level, int* isLast);
void Tree_displayLevelOrder(TreeNode* root);
void Tree_displaySubtree(TreeNode* node, const char* name);
void Tree_calculateStats(TreeNode* node, int* stats); 
void Tree_saveToJSON(TreeNode* node, cJSON* json);
void Tree_free(TreeNode* node);

#endif