#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

extern StackNode* deleteStack;
extern HistoryNode* historyHead;

TreeNode* Tree_createNode(const char* name, const char* type) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        printf("Alokasi memori gagal!\n");
        exit(1);
    }
    strcpy(node->name, name);
    strcpy(node->type, type);
    node->children = NULL;
    node->childCount = 0;
    return node;
}

TreeNode* Tree_parseJSON(cJSON* json) {
    if (!json) return NULL;
    
    cJSON* name = cJSON_GetObjectItem(json, "name");
    cJSON* type = cJSON_GetObjectItem(json, "type");
    if (!name || !type) {
        printf("Struktur JSON tidak valid!\n");
        return NULL;
    }
    
    TreeNode* node = Tree_createNode(name->valuestring, type->valuestring);
    
    cJSON* children = cJSON_GetObjectItem(json, "children");
    if (children && cJSON_IsArray(children)) {
        node->childCount = cJSON_GetArraySize(children);
        node->children = (TreeNode**)malloc(node->childCount * sizeof(TreeNode*));
        if (!node->children) {
            printf("Alokasi memori gagal!\n");
            exit(1);
        }
        for (int i = 0; i < node->childCount; i++) {
            node->children[i] = Tree_parseJSON(cJSON_GetArrayItem(children, i));
        }
    }
    return node;
}

void Tree_addNode(TreeNode* parent, TreeNode* newNode) {
    parent->childCount++;
    parent->children = (TreeNode**)realloc(parent->children, parent->childCount * sizeof(TreeNode*));
    if (!parent->children) {
        printf("Alokasi memori gagal!\n");
        exit(1);
    }
    parent->children[parent->childCount - 1] = newNode;
}

TreeNode* Tree_findNode(TreeNode* node, const char* name) {
    if (!node) return NULL;
    if (strcmp(node->name, name) == 0) return node;
    
    for (int i = 0; i < node->childCount; i++) {
        TreeNode* result = Tree_findNode(node->children[i], name);
        if (result) return result;
    }
    return NULL;
}

TreeNode* Tree_findParent(TreeNode* node, TreeNode* target) {
    if (!node || !target) return NULL;
    
    for (int i = 0; i < node->childCount; i++) {
        if (node->children[i] == target) {
            return node;
        }
        TreeNode* parent = Tree_findParent(node->children[i], target);
        if (parent) return parent;
    }
    return NULL;
}
