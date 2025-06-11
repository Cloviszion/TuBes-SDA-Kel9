#ifndef CJSON_H
#define CJSON_H

/* tipe cJSON */
#define cJSON_False  (1 << 0)
#define cJSON_True   (1 << 1)
#define cJSON_NULL   (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array  (1 << 5)
#define cJSON_Object (1 << 6)

/* struktur cJSON  */
typedef struct cJSON {
    struct cJSON *next, *prev; /* buat linked list di array/objek */
    struct cJSON *child;       /* Children di array/objek */
    int type;                  /* tipe item (cJSON_False, dll.) */
    char *valuestring;         /* String value */
    double valuedouble;        /* Number value */
    char *string;              /* nama item (key di objek) */
} cJSON;

/* prototip fungsi */
cJSON *cJSON_Parse(const char *value);
void cJSON_Delete(cJSON *item);
cJSON *cJSON_CreateObject(void);
cJSON *cJSON_CreateArray(void);
void cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
void cJSON_AddItemToArray(cJSON *array, cJSON *item);
cJSON *cJSON_CreateString(const char *string);
char *cJSON_Print(cJSON *item);
cJSON *cJSON_GetObjectItem(cJSON *object, const char *string);
int cJSON_Getarrayize(cJSON *array);
cJSON *cJSON_GetArrayItem(cJSON *array, int index);
int cJSON_IsArray(cJSON *item);

#endif
