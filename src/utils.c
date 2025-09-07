/*
 * INGRES ChatBot - Utility Library Implementation
 * High-performance, easy-to-use utility functions
 */

#include "utils.h"
#include <stdarg.h>
#include <errno.h>

// Global error state
static UtilsError last_error = UTILS_SUCCESS;

// ============================================================================
// ERROR HANDLING
// ============================================================================

UtilsError utils_get_last_error(void) {
    return last_error;
}

const char* utils_get_error_message(UtilsError error) {
    switch (error) {
        case UTILS_SUCCESS: return "No error";
        case UTILS_ERROR_NULL_POINTER: return "Null pointer error";
        case UTILS_ERROR_OUT_OF_MEMORY: return "Out of memory";
        case UTILS_ERROR_FILE_NOT_FOUND: return "File not found";
        case UTILS_ERROR_INVALID_INPUT: return "Invalid input";
        case UTILS_ERROR_BUFFER_OVERFLOW: return "Buffer overflow";
        default: return "Unknown error";
    }
}

// ============================================================================
// STRING UTILITIES
// ============================================================================

string get_string(const char* prompt) {
    if (prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }

    char* buffer = malloc(1024);
    if (!buffer) {
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    if (fgets(buffer, 1024, stdin) == NULL) {
        free(buffer);
        return NULL;
    }

    // Remove newline
    buffer[strcspn(buffer, "\n")] = 0;

    // Resize to actual length
    string result = malloc(strlen(buffer) + 1);
    if (!result) {
        free(buffer);
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    strcpy(result, buffer);
    free(buffer);
    return result;
}

string get_string_nonempty(const char* prompt) {
    string result;
    do {
        result = get_string(prompt);
        if (result && strlen(result) > 0) {
            return result;
        }
        if (result) free(result);
        printf("Please enter a non-empty string.\n");
    } while (true);
}

string create_string(const char* initial_value) {
    if (!initial_value) {
        last_error = UTILS_ERROR_NULL_POINTER;
        return NULL;
    }

    string result = malloc(strlen(initial_value) + 1);
    if (!result) {
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    strcpy(result, initial_value);
    return result;
}

string format_string(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // Calculate required size
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, format, args_copy) + 1;
    va_end(args_copy);

    string result = malloc(size);
    if (!result) {
        va_end(args);
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    vsnprintf(result, size, format, args);
    va_end(args);
    return result;
}

string string_concat(const string str1, const string str2) {
    if (!str1 || !str2) {
        last_error = UTILS_ERROR_NULL_POINTER;
        return NULL;
    }

    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    string result = malloc(len1 + len2 + 1);

    if (!result) {
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

string string_duplicate(const string src) {
    if (!src) {
        last_error = UTILS_ERROR_NULL_POINTER;
        return NULL;
    }
    return create_string(src);
}

string string_substring(const string src, int start, int length) {
    if (!src || start < 0 || length < 0) {
        last_error = UTILS_ERROR_INVALID_INPUT;
        return NULL;
    }

    int src_len = strlen(src);
    if (start >= src_len) {
        return create_string("");
    }

    if (start + length > src_len) {
        length = src_len - start;
    }

    string result = malloc(length + 1);
    if (!result) {
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    strncpy(result, src + start, length);
    result[length] = '\0';
    return result;
}

string string_trim(const string src) {
    if (!src) return NULL;

    // Find start
    const char* start = src;
    while (isspace(*start)) start++;

    // Find end
    const char* end = src + strlen(src) - 1;
    while (end > start && isspace(*end)) end--;

    // Create result
    int length = end - start + 1;
    string result = malloc(length + 1);
    if (!result) {
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    strncpy(result, start, length);
    result[length] = '\0';
    return result;
}

string string_to_lower(const string src) {
    if (!src) return NULL;

    string result = string_duplicate(src);
    if (!result) return NULL;

    for (int i = 0; result[i]; i++) {
        result[i] = tolower(result[i]);
    }
    return result;
}

bool string_contains(const string haystack, const string needle) {
    if (!haystack || !needle) return false;
    return strstr(haystack, needle) != NULL;
}

bool string_starts_with(const string str, const string prefix) {
    if (!str || !prefix) return false;
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool string_ends_with(const string str, const string suffix) {
    if (!str || !suffix) return false;

    int str_len = strlen(str);
    int suffix_len = strlen(suffix);

    if (suffix_len > str_len) return false;

    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

bool string_equals(const string str1, const string str2) {
    if (!str1 || !str2) return false;
    return strcmp(str1, str2) == 0;
}

bool string_equals_ignore_case(const string str1, const string str2) {
    if (!str1 || !str2) return false;

    string lower1 = string_to_lower(str1);
    string lower2 = string_to_lower(str2);

    bool result = string_equals(lower1, lower2);

    free(lower1);
    free(lower2);
    return result;
}

int string_length(const string str) {
    return str ? strlen(str) : 0;
}

bool string_is_empty(const string str) {
    return !str || strlen(str) == 0;
}

// ============================================================================
// STRING ARRAYS
// ============================================================================

StringArray* string_array_create(void) {
    StringArray* arr = malloc(sizeof(StringArray));
    if (!arr) {
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    arr->items = malloc(sizeof(string) * 10);
    if (!arr->items) {
        free(arr);
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    arr->count = 0;
    arr->capacity = 10;
    return arr;
}

void string_array_add(StringArray* arr, const string item) {
    if (!arr || !item) return;

    // Resize if needed
    if (arr->count >= arr->capacity) {
        arr->capacity *= 2;
        arr->items = realloc(arr->items, sizeof(string) * arr->capacity);
        if (!arr->items) {
            last_error = UTILS_ERROR_OUT_OF_MEMORY;
            return;
        }
    }

    arr->items[arr->count] = string_duplicate(item);
    arr->count++;
}

string string_array_get(StringArray* arr, int index) {
    if (!arr || index < 0 || index >= arr->count) {
        return NULL;
    }
    return arr->items[index];
}

void string_array_free(StringArray* arr) {
    if (!arr) return;

    for (int i = 0; i < arr->count; i++) {
        free(arr->items[i]);
    }
    free(arr->items);
    free(arr);
}

StringArray* string_split(const string str, const char delimiter) {
    if (!str) return NULL;

    StringArray* result = string_array_create();
    if (!result) return NULL;

    string str_copy = string_duplicate(str);
    char delim_str[2] = {delimiter, '\0'};

    char* token = strtok(str_copy, delim_str);
    while (token != NULL) {
        string trimmed = string_trim(token);
        if (!string_is_empty(trimmed)) {
            string_array_add(result, trimmed);
        }
        free(trimmed);
        token = strtok(NULL, delim_str);
    }

    free(str_copy);
    return result;
}

// ============================================================================
// HASH TABLE
// ============================================================================

static unsigned int hash_function(const string key) {
    unsigned int hash = 5381;
    for (int i = 0; key[i]; i++) {
        hash = ((hash << 5) + hash) + key[i];
    }
    return hash;
}

HashTable* hash_create(void) {
    HashTable* table = malloc(sizeof(HashTable));
    if (!table) {
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    table->bucket_count = 101; // Prime number for better distribution
    table->buckets = calloc(table->bucket_count, sizeof(HashNode*));
    if (!table->buckets) {
        free(table);
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    table->size = 0;
    return table;
}

void hash_set(HashTable* table, const string key, void* value) {
    if (!table || !key) return;

    unsigned int index = hash_function(key) % table->bucket_count;
    HashNode* node = table->buckets[index];

    // Check if key already exists
    while (node) {
        if (string_equals(node->key, key)) {
            node->value = value;
            return;
        }
        node = node->next;
    }

    // Create new node
    HashNode* new_node = malloc(sizeof(HashNode));
    if (!new_node) {
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return;
    }

    new_node->key = string_duplicate(key);
    new_node->value = value;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->size++;
}

void* hash_get(HashTable* table, const string key) {
    if (!table || !key) return NULL;

    unsigned int index = hash_function(key) % table->bucket_count;
    HashNode* node = table->buckets[index];

    while (node) {
        if (string_equals(node->key, key)) {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

bool hash_contains(HashTable* table, const string key) {
    return hash_get(table, key) != NULL;
}

void hash_free(HashTable* table) {
    if (!table) return;

    for (int i = 0; i < table->bucket_count; i++) {
        HashNode* node = table->buckets[i];
        while (node) {
            HashNode* next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
    }

    free(table->buckets);
    free(table);
}

// ============================================================================
// INPUT FUNCTIONS
// ============================================================================

int get_int(const char* prompt) {
    string input = get_string(prompt);
    if (!input) return 0;

    int result = atoi(input);
    free(input);
    return result;
}

float get_float(const char* prompt) {
    string input = get_string(prompt);
    if (!input) return 0.0f;

    float result = atof(input);
    free(input);
    return result;
}

bool get_bool(const char* prompt) {
    string input = get_string(prompt);
    if (!input) return false;

    string lower = string_to_lower(input);
    bool result = string_equals(lower, "yes") ||
                  string_equals(lower, "y") ||
                  string_equals(lower, "true") ||
                  string_equals(lower, "1");

    free(input);
    free(lower);
    return result;
}

// ============================================================================
// OUTPUT FUNCTIONS
// ============================================================================

void print_colored(const char* color, const char* format, ...) {
    printf("%s", color);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("%s", COLOR_RESET);
}

void print_success(const char* format, ...) {
    printf("%s✅ ", COLOR_GREEN);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("%s\n", COLOR_RESET);
}

void print_error(const char* format, ...) {
    printf("%s❌ ", COLOR_RED);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("%s\n", COLOR_RESET);
}

void print_warning(const char* format, ...) {
    printf("%s⚠️  ", COLOR_YELLOW);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("%s\n", COLOR_RESET);
}

void print_info(const char* format, ...) {
    printf("%sℹ️  ", COLOR_BLUE);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("%s\n", COLOR_RESET);
}

// ============================================================================
// FILE OPERATIONS
// ============================================================================

string file_read_all(const string filename) {
    if (!filename) return NULL;

    FILE* file = fopen(filename, "r");
    if (!file) {
        last_error = UTILS_ERROR_FILE_NOT_FOUND;
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer
    string content = malloc(size + 1);
    if (!content) {
        fclose(file);
        last_error = UTILS_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    // Read file
    size_t read_size = fread(content, 1, size, file);
    content[read_size] = '\0';

    fclose(file);
    return content;
}

bool file_write_all(const string filename, const string content) {
    if (!filename || !content) return false;

    FILE* file = fopen(filename, "w");
    if (!file) {
        last_error = UTILS_ERROR_FILE_NOT_FOUND;
        return false;
    }

    size_t written = fwrite(content, 1, strlen(content), file);
    fclose(file);

    return written == strlen(content);
}

bool file_exists(const string filename) {
    if (!filename) return false;

    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// ============================================================================
// CSV OPERATIONS
// ============================================================================

StringArray* csv_read_line(const string csv_line) {
    if (!csv_line) return NULL;

    StringArray* fields = string_array_create();
    if (!fields) return NULL;

    string line_copy = string_duplicate(csv_line);
    char* token = strtok(line_copy, ",");

    while (token != NULL) {
        string trimmed = string_trim(token);
        string_array_add(fields, trimmed);
        free(trimmed);
        token = strtok(NULL, ",");
    }

    free(line_copy);
    return fields;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

int min_int(int a, int b) {
    return a < b ? a : b;
}

int max_int(int a, int b) {
    return a > b ? a : b;
}

string get_current_time_string(void) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);

    string result = malloc(64);
    if (!result) return NULL;

    strftime(result, 64, "%H:%M:%S", tm_info);
    return result;
}

string get_current_date_string(void) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);

    string result = malloc(64);
    if (!result) return NULL;

    strftime(result, 64, "%Y-%m-%d", tm_info);
    return result;
}

bool is_valid_number(const string str) {
    if (!str || string_is_empty(str)) return false;

    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i]) && str[i] != '.' && str[i] != '-') {
            return false;
        }
    }
    return true;
}

void debug_print(const char* format, ...) {
    #ifdef DEBUG
    printf("[DEBUG] ");
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    #endif
}