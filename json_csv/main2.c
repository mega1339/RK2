#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define MAX_LINE_LENGTH 1024
#define MAX_FIELD_LENGTH 256

void parseJSON(cJSON* root, FILE* file) {
    cJSON* jsonData = cJSON_GetArrayItem(root, 0);
    cJSON* fields = cJSON_GetObjectItem(jsonData, "fields");
    cJSON* data = cJSON_GetObjectItem(jsonData, "data");

    // Записываем имена полей в первую строку CSV
    cJSON* field = NULL;
    cJSON_ArrayForEach(field, fields) {
        fprintf(file, "\"%s\",", field->valuestring);
    }
    fprintf(file, "\n");

    // Записываем значения полей для каждой записи в CSV
    cJSON* record = NULL;
    cJSON_ArrayForEach(record, data) {
        cJSON* value = NULL;
        cJSON_ArrayForEach(value, record) {
            fprintf(file, "\"%s\",", value->valuestring);
        }
        fprintf(file, "\n");
    }
}

int main() {
    FILE* jsonFile = fopen("input.json", "r");
    if (!jsonFile) {
        printf("Failed to open JSON file.\n");
        return 1;
    }

    // Читаем содержимое JSON файла
    fseek(jsonFile, 0, SEEK_END);
    long fileSize = ftell(jsonFile);
    fseek(jsonFile, 0, SEEK_SET);
    char* jsonBuffer = (char*)malloc(fileSize + 1);
    fread(jsonBuffer, 1, fileSize, jsonFile);
    fclose(jsonFile);
    jsonBuffer[fileSize] = '\0';

    // Разбираем JSON
    cJSON* root = cJSON_Parse(jsonBuffer);
    free(jsonBuffer);
    if (!root) {
        printf("Failed to parse JSON.\n");
        return 1;
    }

    FILE* csvFile = fopen("output.csv", "a");
    if (!csvFile) {
        printf("Failed to open CSV file.\n");
        cJSON_Delete(root);
        return 1;
    }

    // Преобразуем JSON в CSV
    parseJSON(root, csvFile);

    fclose(csvFile);
    cJSON_Delete(root);

    return 0;
}
