#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define MAX_LINE_LENGTH 1024
#define MAX_FIELD_LENGTH 256

cJSON* parseCSV(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    cJSON* root = cJSON_CreateArray();
    cJSON* fields = NULL;
    int isFirstLine = 1;

    while (fgets(line, sizeof(line), file)) {
        // Удаляем символ новой строки
        line[strcspn(line, "\n")] = '\0';

        char* token;
        char* field;
        int fieldIndex = 0;
        cJSON* item = cJSON_CreateObject();

        // Разделяем строку по запятым
        token = strtok(line, ",");
        while (token != NULL) {
            field = (char*)malloc(MAX_FIELD_LENGTH);
            strncpy(field, token, MAX_FIELD_LENGTH);

            if (isFirstLine) {
                // Создаем массив для имен полей
                if (!fields) {
                    fields = cJSON_CreateArray();
                }

                // Добавляем имя поля в массив fields
                cJSON_AddItemToArray(fields, cJSON_CreateString(field));
            } else {
                // Добавляем значение в объект item
                cJSON_AddItemToObject(item, cJSON_GetArrayItem(fields, fieldIndex)->valuestring, cJSON_CreateString(field));
                fieldIndex++;
            }

            token = strtok(NULL, ",");
            free(field);
        }

        if (!isFirstLine) {
            // Добавляем объект item в массив root
            cJSON_AddItemToArray(root, item);
        }

        isFirstLine = 0;
    }

    fclose(file);
    cJSON_AddItemToObject(root, "fields", fields); // Добавляем массив fields в корневой объект root
    return root;
}

int main() {
    cJSON* json = parseCSV("example.csv");
    if (json) {
        FILE* jsonFile = fopen("output.json", "w");
        if (jsonFile) {
            char* jsonString = cJSON_Print(json);
            fprintf(jsonFile, "%s\n", jsonString);
            fclose(jsonFile);
            cJSON_free(jsonString);
        } else {
            printf("Failed to open output file.\n");
        }
        
        cJSON_Delete(json);
    }

    return 0;
}
