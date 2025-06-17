#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "history.h"
#include "tree.h"
#include "queue.h"
#include "stack.h"

TreeNode* root = NULL;
QueueNode* queueHead = NULL;
StackNode* addStack = NULL;
StackNode* redoStack = NULL;
StackNode* deleteStack = NULL;
HistoryNode* historyHead = NULL;

void initTree() {
    if (root) {
        printf("Pohon sudah diinisialisasi!\n");
        return;
    }
    
    FILE* file = fopen("jawabarat_hierarchy.json", "r");
    if (!file) {
        printf("Gagal membuka file JSON!\n");
        return;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    if (length <= 0) {
        printf("File JSON kosong!\n");
        fclose(file);
        return;
    }
    fseek(file, 0, SEEK_SET);
    char* data = (char*)malloc(length + 1);
    if (!data) {
        printf("Alokasi memori gagal!\n");
        fclose(file);
        exit(1);
    }
    size_t read_bytes = fread(data, 1, length, file);
    data[read_bytes] = '\0';
    fclose(file);
    
    cJSON* json = cJSON_Parse(data);
    if (!json) {
        printf("Gagal memparsing JSON.\n");
        free(data);
        return;
    }
    
    root = Tree_parseJSON(json);
    cJSON_Delete(json);
    free(data);
    if (root) {
        History_add(&historyHead, "Menginisialisasi pohon dari JSON");
        printf("Pohon berhasil diinisialisasi!\n");
    } else {
        printf("Gagal memparsing pohon dari JSON.\n");
    }
}

void addToQueue() {
    char name[100], type[20], parentName[100];
    printf("Masukkan nama entitas: ");
    scanf(" %[^\n]", name);
    printf("Masukkan tipe entitas (provinsi/kabupaten/kota/kecamatan/kelurahan/desa/rw/rt): ");
    scanf(" %[^\n]", type);
    printf("Masukkan nama parent: ");
    scanf(" %[^\n]", parentName);
    
    Queue_add(&queueHead, name, type, parentName, root, History_add, Stack_pushAdd);
}

void manageQueue() {
    int choice;
    do {
        printf("\nSubmenu Pengelolaan Antrean\n");
        printf("1. Proses Data ke Pohon/JSON\n");
        printf("2. Edit Data di Antrean\n");
        printf("3. Hapus Data dari Antrean\n");
        printf("4. Tampilkan Antrean\n");
        printf("5. Kembali ke Menu Utama\n");
        printf("Masukkan pilihan: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: Queue_processToTree(&queueHead, root, History_add); break;
            case 2: {
                char name[100];
                printf("Masukkan nama entitas untuk diedit: ");
                scanf(" %[^\n]", name);
                Queue_edit(queueHead, name, History_add);
                break;
            }
            case 3: {
                char name[100];
                printf("Masukkan nama entitas untuk dihapus: ");
                scanf(" %[^\n]", name);
                Queue_delete(&queueHead, name, History_add);
                break;
            }
            case 4: Queue_display(queueHead); break;
            case 5: break;
            default: printf("Pilihan tidak valid!\n");
        }
    } while (choice != 5);
}

void deleteNode() {
    char name[100];
    printf("Masukkan nama node untuk dihapus: ");
    scanf(" %[^\n]", name);
    Tree_deleteNode(&root, name, History_add, Stack_pushDelete);
}

void editNode() {
    char name[100], newName[100], newType[20];
    printf("Masukkan nama node untuk diedit: ");
    scanf(" %[^\n]", name);
    printf("Masukkan nama baru: ");
    scanf(" %[^\n]", newName);
    printf("Masukkan tipe baru: ");
    scanf(" %[^\n]", newType);
    Tree_editNode(root, name, newName, newType, History_add);
}

void searchNode() {
    char name[100];
    printf("Masukkan nama node untuk dicari: ");
    scanf(" %[^\n]", name);
    Tree_searchNode(root, name);
}

void displaySubtree() {
    char name[100];
    printf("Masukkan nama node untuk menampilkan subtree: ");
    scanf(" %[^\n]", name);
    Tree_displaySubtree(root, name);
}

void displayStats() {
    int stats[7] = {0}; // provinsi, kabupaten, kota, kecamatan, kelurahan/desa, rw, rt
    Tree_calculateStats(root, stats);
    printf("Statistik:\n");
    printf("Provinsi: %d\n", stats[0]);
    printf("Kabupaten: %d\n", stats[1]);
    printf("Kota: %d\n", stats[2]);
    printf("Kecamatan: %d\n", stats[3]);
    printf("Kelurahan/Desa: %d\n", stats[4]);
    printf("RW: %d\n", stats[5]);
    printf("RT: %d\n", stats[6]);
}

void saveTreeToJSON() {
    if (!root) {
        printf("Pohon kosong!\n");
        return;
    }
    
    cJSON* json = cJSON_CreateObject();
    Tree_saveToJSON(root, json);
    
    char* output = cJSON_Print(json);
    FILE* file = fopen("jawabarat_updated.json", "w");
    if (!file) {
        printf("Gagal membuka file output!\n");
        cJSON_Delete(json);
        free(output);
        return;
    }
    
    fprintf(file, "%s", output);
    fclose(file);
    cJSON_Delete(json);
    free(output);
    
    History_add(&historyHead, "Menyimpan pohon ke jawabarat_updated.json");
    printf("Pohon disimpan ke jawabarat_updated.json!\n");
}

void cleanup() {
    Tree_free(root);
    Queue_free(queueHead);
    Stack_free(addStack);
    Stack_free(redoStack);
    Stack_free(deleteStack);
    History_free(historyHead);
}

int main() {
    int choice;
    do {
        printf("\nMenu Utama\n");
        printf("1. Inisialisasi Pohon dari JSON\n");
        printf("2. Tambah Entitas Baru\n");
        printf("3. Kelola Antrean\n");
        printf("4. Hapus Entitas\n");
        printf("5. Edit Entitas\n");
        printf("6. Cari Data\n");
        printf("7. Tampilkan Hierarki (Preorder)\n");
        printf("8. Tampilkan Hierarki (Level-Order)\n");
        printf("9. Tampilkan Subtree\n");
        printf("10. Hitung Statistik\n");
        printf("11. Tampilkan Riwayat Operasi\n");
        printf("12. Batalkan Tambah\n");
        printf("13. Ulang Tambah\n");
        printf("14. Batalkan Hapus\n");
        printf("15. Simpan ke JSON\n");
        printf("16. Keluar\n");
        printf("Masukkan pilihan: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: initTree(); break;
            case 2: addToQueue(); break;
            case 3: manageQueue(); break;
            case 4: deleteNode(); break;
            case 5: editNode(); break;
            case 6: searchNode(); break;
            case 7: {
                if (root) {
                    int isLast[100];
                    Tree_displayPreorder(root, 0, isLast);
                } else {
                    printf("Pohon kosong!\n");
                }
                break;
            }
            case 8: Tree_displayLevelOrder(root); break;
            case 9: displaySubtree(); break;
            case 10: displayStats(); break;
            case 11: History_display(historyHead); break;
            case 12: Stack_undoAdd(&addStack, &redoStack, &queueHead, root, History_add); break;
            case 13: Stack_redoAdd(&redoStack, &addStack, &queueHead, root, History_add); break;
            case 14: Stack_undoDelete(&deleteStack, &redoStack, root, History_add); break;
            case 15: saveTreeToJSON(); break;
            case 16: cleanup(); printf("Keluar...\n"); break;
            default: printf("Pilihan tidak valid!\n");
        }
    } while (choice != 16);
    
    return 0;
}