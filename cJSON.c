#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cJSON.h"

/* parsing Internal  */
typedef struct {
    const char *str;
    int pos;
} ParseState;

/* skip whitespace */
static void skipWhitespace(ParseState *state) {
    while (state->str[state->pos] && isspace((unsigned char)state->str[state->pos])) {
        state->pos++;
    }
}

/* Parse string */
static char *parseString(ParseState *state) {
    if (state->str[state->pos] != '"') return NULL;
    state->pos++;
    int start = state->pos;
    while (state->str[state->pos] && state->str[state->pos] != '"') {
        if (state->str[state->pos] == '\\') state->pos++; /* Skip escape */
        state->pos++;
    }
    if (!state->str[state->pos]) return NULL;
    int len = state->pos - start;
    char *result = (char *)malloc(len + 1);
    if (!result) return NULL;
    strncpy(result, state->str + start, len);
    result[len] = '\0';
    state->pos++; /* Skip closing quote */
    return result;
}

/* Parse nomor */
static double parseNumber(ParseState *state) {
    char buffer[32];
    int i = 0;
    while (state->str[state->pos] && (isdigit((unsigned char)state->str[state->pos]) || state->str[state->pos] == '.' || state->str[state->pos] == '-' || state->str[state->pos] == 'e' || state->str[state->pos] == 'E')) {
        buffer[i++] = state->str[state->pos++];
        if (i >= 31) break;
    }
    buffer[i] = '\0';
    return atof(buffer);
}

/* Create cJSON item */
static cJSON *createItem(int type) {
    cJSON *item = (cJSON *)malloc(sizeof(cJSON));
    if (!item) return NULL;
    memset(item, 0, sizeof(cJSON));
    item->type = type;
    return item;
}

/* Parse JSON value */
static cJSON *parseValue(ParseState *state);

/* Parse array */
static cJSON *parseArray(ParseState *state) {
    cJSON *array = createItem(cJSON_Array);
    if (!array) return NULL;
    state->pos++; /* Skip [ */
    skipWhitespace(state);
    if (state->str[state->pos] == ']') {
        state->pos++;
        return array;
    }
    cJSON *last = NULL;
    while (state->str[state->pos]) {
        cJSON *item = parseValue(state);
        if (!item) {
            cJSON_Delete(array);
            return NULL;
        }
        if (!array->child) {
            array->child = item;
        } else {
            last->next = item;
            item->prev = last;
        }
        last = item;
        skipWhitespace(state);
        if (state->str[state->pos] == ']') {
            state->pos++;
            break;
        }
        if (state->str[state->pos] != ',') {
            cJSON_Delete(array);
            return NULL;
        }
        state->pos++;
        skipWhitespace(state);
    }
    return array;
}

/* Parse objek */
static cJSON *parseObject(ParseState *state) {
    cJSON *object = createItem(cJSON_Object);
    if (!object) return NULL;
    state->pos++; /* Skip { */
    skipWhitespace(state);
    if (state->str[state->pos] == '}') {
        state->pos++;
        return object;
    }
    cJSON *last = NULL;
    while (state->str[state->pos]) {
        char *key = parseString(state);
        if (!key) {
            cJSON_Delete(object);
            return NULL;
        }
        skipWhitespace(state);
        if (state->str[state->pos] != ':') {
            free(key);
            cJSON_Delete(object);
            return NULL;
        }
        state->pos++;
        skipWhitespace(state);
        cJSON *item = parseValue(state);
        if (!item) {
            free(key);
            cJSON_Delete(object);
            return NULL;
        }
        item->string = key;
        if (!object->child) {
            object->child = item;
        } else {
            last->next = item;
            item->prev = last;
        }
        last = item;
        skipWhitespace(state);
        if (state->str[state->pos] == '}') {
            state->pos++;
            break;
        }
        if (state->str[state->pos] != ',') {
            cJSON_Delete(object);
            return NULL;
        }
        state->pos++;
        skipWhitespace(state);
    }
    return object;
}

/* Parse JSON value */
static cJSON *parseValue(ParseState *state) {
    skipWhitespace(state);
    char c = state->str[state->pos];
    if (c == '{') return parseObject(state);
    if (c == '[') return parseArray(state);
    if (c == '"') {
        char *str = parseString(state);
        if (!str) return NULL;
        cJSON *item = createItem(cJSON_String);
        if (!item) {
            free(str);
            return NULL;
        }
        item->valuestring = str;
        return item;
    }
    if (isdigit((unsigned char)c) || c == '-') {
        cJSON *item = createItem(cJSON_Number);
        if (!item) return NULL;
        item->valuedouble = parseNumber(state);
        return item;
    }
    if (strncmp(state->str + state->pos, "true", 4) == 0) {
        state->pos += 4;
        return createItem(cJSON_True);
    }
    if (strncmp(state->str + state->pos, "false", 5) == 0) {
        state->pos += 5;
        return createItem(cJSON_False);
    }
    if (strncmp(state->str + state->pos, "null", 4) == 0) {
        state->pos += 4;
        return createItem(cJSON_NULL);
    }
    return NULL;
}

/* Parse JSON string */
cJSON *cJSON_Parse(const char *value) {
    ParseState state = {value, 0};
    cJSON *result = parseValue(&state);
    skipWhitespace(&state);
    if (state.str[state.pos]) {
        cJSON_Delete(result);
        return NULL;
    }
    return result;
}

/* melepas struktur cJSON */
void cJSON_Delete(cJSON *item) {
    if (!item) return;
    cJSON *current = item->child;
    while (current) {
        cJSON *next = current->next;
        cJSON_Delete(current);
        current = next;
    }
    if (item->valuestring) free(item->valuestring);
    if (item->string) free(item->string);
    free(item);
}

/* Create JSON object */
cJSON *cJSON_CreateObject(void) {
    return createItem(cJSON_Object);
}

/* Create JSON array */
cJSON *cJSON_CreateArray(void) {
    return createItem(cJSON_Array);
}

/* Add item ke objek */
void cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item) {
    if (!object || !item || !string) return;
    item->string = (char *)malloc(strlen(string) + 1);
    if (!item->string) return;
    strcpy(item->string, string);
    if (!object->child) {
        object->child = item;
    } else {
        cJSON *last = object->child;
        while (last->next) last = last->next;
        last->next = item;
        item->prev = last;
    }
}

/* Add item ke array */
void cJSON_AddItemToArray(cJSON *array, cJSON *item) {
    if (!array || !item) return;
    if (!array->child) {
        array->child = item;
    } else {
        cJSON *last = array->child;
        while (last->next) last = last->next;
        last->next = item;
        item->prev = last;
    }
}

/* Create string item */
cJSON *cJSON_CreateString(const char *string) {
    cJSON *item = createItem(cJSON_String);
    if (!item) return NULL;
    item->valuestring = (char *)malloc(strlen(string) + 1);
    if (!item->valuestring) {
        free(item);
        return NULL;
    }
    strcpy(item->valuestring, string);
    return item;
}

/* Print JSON ke string */
static void printJSON(cJSON *item, char *buffer, int *pos, int depth) {
    if (!item) return;
    int i;
    for (i = 0; i < depth; i++) *pos += sprintf(buffer + *pos, "  ");
    if (item->type == cJSON_Object) {
        *pos += sprintf(buffer + *pos, "{\n");
        cJSON *child = item->child;
        while (child) {
            for (i = 0; i < depth + 1; i++) *pos += sprintf(buffer + *pos, "  ");
            *pos += sprintf(buffer + *pos, "\"%s\": ", child->string);
            printJSON(child, buffer, pos, 0);
            if (child->next) *pos += sprintf(buffer + *pos, ",");
            *pos += sprintf(buffer + *pos, "\n");
            child = child->next;
        }
        for (i = 0; i < depth; i++) *pos += sprintf(buffer + *pos, "  ");
        *pos += sprintf(buffer + *pos, "}");
    } else if (item->type == cJSON_Array) {
        *pos += sprintf(buffer + *pos, "[\n");
        cJSON *child = item->child;
        while (child) {
            printJSON(child, buffer, pos, depth + 1);
            if (child->next) *pos += sprintf(buffer + *pos, ",");
            *pos += sprintf(buffer + *pos, "\n");
            child = child->next;
        }
        for (i = 0; i < depth; i++) *pos += sprintf(buffer + *pos, "  ");
        *pos += sprintf(buffer + *pos, "]");
    } else if (item->type == cJSON_String) {
        *pos += sprintf(buffer + *pos, "\"%s\"", item->valuestring);
    } else if (item->type == cJSON_Number) {
        *pos += sprintf(buffer + *pos, "%g", item->valuedouble);
    } else if (item->type == cJSON_True) {
        *pos += sprintf(buffer + *pos, "true");
    } else if (item->type == cJSON_False) {
        *pos += sprintf(buffer + *pos, "false");
    } else if (item->type == cJSON_NULL) {
        *pos += sprintf(buffer + *pos, "null");
    }
}

char *cJSON_Print(cJSON *item) {
    char *buffer = (char *)malloc(1024 * 1024); /* 1MB buffer */
    if (!buffer) return NULL;
    int pos = 0;
    printJSON(item, buffer, &pos, 0);
    buffer[pos] = '\0';
    return buffer;
}

/* cari berdasarkan key  */
cJSON *cJSON_GetObjectItem(cJSON *object, const char *string) {
    if (!object || !string) return NULL;
    cJSON *child = object->child;
    while (child) {
        if (child->string && strcmp(child->string, string) == 0) return child;
        child = child->next;
    }
    return NULL;
}

/* Get ukuran array */
int cJSON_GetArraySize(cJSON *array) {
    if (!array || array->type != cJSON_Array) return 0;
    int size = 0;
    cJSON *child = array->child;
    while (child) {
        size++;
        child = child->next;
    }
    return size;
}

/* Get item array berdasarkan index */
cJSON *cJSON_GetArrayItem(cJSON *array, int index) {
    if (!array || array->type != cJSON_Array) return NULL;
    cJSON *child = array->child;
    while (child && index > 0) {
        child = child->next;
        index--;
    }
    return child;
}

/* Cheking array */
int cJSON_IsArray(cJSON *item) {
    return item && item->type == cJSON_Array;
}
