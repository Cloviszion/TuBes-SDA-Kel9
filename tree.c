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

void Tree_deleteNode(TreeNode** root, const char* name, void (*addToHistory)(HistoryNode**, const char*), void (*pushDeleteStack)(StackNode**, TreeNode*, TreeNode*, const char*)) {
    if (!*root) {
        printf("Pohon kosong!\n");
        return;
    }
    
    if (strcmp((*root)->name, name) == 0) {
        printf("Tidak dapat menghapus node akar!\n");
        return;
    }
    
    TreeNode* target = Tree_findNode(*root, name);
    if (!target) {
        printf("Node tidak ditemukan!\n");
        return;
    }
    
    TreeNode* parent = Tree_findParent(*root, target);
    if (!parent) {
        printf("Parent tidak ditemukan!\n");
        return;
    }
    
    char operation[200];
    snprintf(operation, sizeof(operation), "Menghapus %s (%s) dari Pohon", target->name, target->type);
    addToHistory(&historyHead, operation);
    pushDeleteStack(&deleteStack, target, parent, operation);
    
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
            printf("Node dihapus!\n");
            return;
        }
    }
}

void Tree_editNode(TreeNode* node, const char* name, const char* newName, const char* newType, void (*addToHistory)(HistoryNode**, const char*)) {
    TreeNode* target = Tree_findNode(node, name);
    if (!target) {
        printf("Node tidak ditemukan!\n");
        return;
    }
    
    char operation[200];
    snprintf(operation, sizeof(operation), "Mengedit %s menjadi %s (%s)", name, newName, newType);
    addToHistory(&historyHead, operation);
    
    strcpy(target->name, newName);
    strcpy(target->type, newType);
    printf("Node diedit!\n");
}

void Tree_searchNode(TreeNode* node, const char* name) {
    TreeNode* target = Tree_findNode(node, name);
    if (!target) {
        printf("Node tidak ditemukan!\n");
        return;
    }
    
    printf("Ditemukan: %s (%s)\n", target->name, target->type);
}

void Tree_displayPreorder(TreeNode* node, int level, int* isLast) {
    if (!node) return;

    for (int i = 0; i < level - 1; i++) {
        if (isLast[i]) {
            printf("   "); 
        } else {
            printf("|  "); 
        }
    }

    if (level > 0) {
        printf("%s_", isLast[level - 1] ? "L" : "|");
        printf(" %s (%s)\n", node->name, node->type);
    } else {
        printf("%s (%s)\n", node->name, node->type);
    }

    for (int i = 0; i < node->childCount; i++) {
        isLast[level] = (i == node->childCount - 1);
        Tree_displayPreorder(node->children[i], level + 1, isLast);
    }
}

void Tree_displayLevelOrder(TreeNode* root) {
    if (!root) {
        printf("Pohon kosong!\n");
        return;
    }
    
    int totalNodes = 0;
    TreeNode* tempQueue[1000];
    int front = 0, rear = 0;
    tempQueue[rear++] = root;
    while (front < rear) {
        TreeNode* node = tempQueue[front++];
        totalNodes++;
        for (int i = 0; i < node->childCount; i++) {
            if (rear < 1000) {
                tempQueue[rear++] = node->children[i];
            }
        }
    }
    
    TreeNode** queue = (TreeNode**)malloc(totalNodes * sizeof(TreeNode*));
    if (!queue) {
        printf("Alokasi memori gagal!\n");
        exit(1);
    }
    front = 0;
    rear = 0;
    queue[rear++] = root;
    
    while (front < rear) {
        TreeNode* node = queue[front++];
        printf("%s (%s)\n", node->name, node->type);
        for (int i = 0; i < node->childCount; i++) {
            queue[rear++] = node->children[i];
        }
    }
    free(queue);
}

void Tree_displaySubtree(TreeNode* node, const char* name) {
    TreeNode* target = Tree_findNode(node, name);
    if (!target) {
        printf("Node tidak ditemukan!\n");
        return;
    }
    
    printf("Subtree dari %s:\n", name);
    int isLast[100];
    Tree_displayPreorder(target, 0, isLast);
}

void Tree_calculateStats(TreeNode* node, int* stats) {
    if (!node) return;
    
    if (strcmp(node->type, "provinsi") == 0) stats[0]++;
    else if (strcmp(node->type, "kabupaten") == 0) stats[1]++;
    else if (strcmp(node->type, "kota") == 0) stats[2]++;
    else if (strcmp(node->type, "kecamatan") == 0) stats[3]++;
    else if (strcmp(node->type, "kelurahan/desa") == 0) stats[4]++;
    else if (strcmp(node->type, "rw") == 0) stats[5]++;
    else if (strcmp(node->type, "rt") == 0) stats[6]++;
    
    for (int i = 0; i < node->childCount; i++) {
        Tree_calculateStats(node->children[i], stats);
    }
}

void Tree_saveToJSON(TreeNode* node, cJSON* json) {
    cJSON_AddItemToObject(json, "name", cJSON_CreateString(node->name));
    cJSON_AddItemToObject(json, "type", cJSON_CreateString(node->type));
    cJSON* children = cJSON_CreateArray();
    cJSON_AddItemToObject(json, "children", children);
    
    for (int i = 0; i < node->childCount; i++) {
        cJSON* child = cJSON_CreateObject();
        cJSON_AddItemToArray(children, child);
        Tree_saveToJSON(node->children[i], child);
    }
}

void Tree_free(TreeNode* node) {
    if (!node) return;
    for (int i = 0; i < node->childCount; i++) {
        Tree_free(node->children[i]);
    }
    free(node->children);
    free(node);
}