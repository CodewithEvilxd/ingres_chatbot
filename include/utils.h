#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>

// ============================================================================
// TYPE DEFINITIONS
// ============================================================================

// String type alias for better readability
typedef char* string;

// Boolean type (stdbool.h included above)

// NULL definition for safety
#ifndef NULL
#define NULL ((void*)0)
#endif

// ============================================================================
// ERROR HANDLING
// ============================================================================

typedef enum {
    UTILS_SUCCESS = 0,
    UTILS_ERROR_NULL_POINTER,
    UTILS_ERROR_OUT_OF_MEMORY,
    UTILS_ERROR_FILE_NOT_FOUND,
    UTILS_ERROR_INVALID_INPUT,
    UTILS_ERROR_BUFFER_OVERFLOW
} UtilsError;

UtilsError utils_get_last_error(void);
const char* utils_get_error_message(UtilsError error);

// ============================================================================
// STRING UTILITIES
// ============================================================================

string get_string(const char* prompt);
string get_string_nonempty(const char* prompt);
string create_string(const char* initial_value);
string format_string(const char* format, ...);
string string_concat(const string str1, const string str2);
string string_duplicate(const string src);
string string_substring(const string src, int start, int length);
string string_trim(const string src);
string string_to_lower(const string src);
bool string_contains(const string haystack, const string needle);
bool string_starts_with(const string str, const string prefix);
bool string_ends_with(const string str, const string suffix);
bool string_equals(const string str1, const string str2);
bool string_equals_ignore_case(const string str1, const string str2);
int string_length(const string str);
bool string_is_empty(const string str);

// ============================================================================
// STRING ARRAYS
// ============================================================================

typedef struct {
    string* items;
    int count;
    int capacity;
} StringArray;

StringArray* string_array_create(void);
void string_array_add(StringArray* arr, const string item);
string string_array_get(StringArray* arr, int index);
void string_array_free(StringArray* arr);
StringArray* string_split(const string str, const char delimiter);

// ============================================================================
// HASH TABLE
// ============================================================================

typedef struct HashNode {
    string key;
    void* value;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode** buckets;
    int bucket_count;
    int size;
} HashTable;

HashTable* hash_create(void);
void hash_set(HashTable* table, const string key, void* value);
void* hash_get(HashTable* table, const string key);
bool hash_contains(HashTable* table, const string key);
void hash_free(HashTable* table);

// ============================================================================
// INPUT FUNCTIONS
// ============================================================================

int get_int(const char* prompt);
float get_float(const char* prompt);
bool get_bool(const char* prompt);

// ============================================================================
// OUTPUT FUNCTIONS
// ============================================================================

// Color definitions for console output
#define COLOR_RESET   "\x1B[0m"
#define COLOR_RED     "\x1B[31m"
#define COLOR_GREEN   "\x1B[32m"
#define COLOR_YELLOW  "\x1B[33m"
#define COLOR_BLUE    "\x1B[34m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN    "\x1B[36m"
#define COLOR_WHITE   "\x1B[37m"

void print_colored(const char* color, const char* format, ...);
void print_success(const char* format, ...);
void print_error(const char* format, ...);
void print_warning(const char* format, ...);
void print_info(const char* format, ...);

// ============================================================================
// FILE OPERATIONS
// ============================================================================

string file_read_all(const string filename);
bool file_write_all(const string filename, const string content);
bool file_exists(const string filename);

// ============================================================================
// CSV OPERATIONS
// ============================================================================

StringArray* csv_read_line(const string csv_line);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

int min_int(int a, int b);
int max_int(int a, int b);
string get_current_time_string(void);
string get_current_date_string(void);
bool is_valid_number(const string str);
void debug_print(const char* format, ...);

#endif // UTILS_H