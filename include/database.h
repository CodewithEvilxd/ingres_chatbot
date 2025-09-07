#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

// Conditionally include PostgreSQL headers
#ifdef USE_POSTGRESQL
#include <libpq-fe.h>
#endif

// Groundwater assessment data structure based on CGWB format
typedef struct {
    char state[50];              // State name
    char district[50];           // District name  
    char block[50];              // Block/Mandal/Taluk name
    char category[20];           // Safe, Semi-Critical, Critical, Over-Exploited
    float annual_recharge;       // Annual groundwater recharge (MCM)
    float extractable_resource;  // Net annual groundwater availability (MCM)
    float annual_extraction;     // Annual groundwater extraction (MCM)
    int assessment_year;         // Year of assessment
} GroundwaterData;

// Query result structure for chatbot responses
typedef struct {
    GroundwaterData* data;       // Array of matching records
    int count;                   // Number of records
    char query_type[50];         // Type of query executed
    float execution_time_ms;     // Query execution time
} QueryResult;

// Database initialization and cleanup
bool db_init(void);
void db_close(void);

// Core query functions
QueryResult* query_by_location(const char* state, const char* district, const char* block);
QueryResult* query_by_state(const char* state);
QueryResult* query_by_category(const char* category);
QueryResult* query_critical_areas(void);
QueryResult* query_historical_trend(const char* state, const char* district, const char* block);

// Utility functions
GroundwaterData* get_location_data(const char* state, const char* district, const char* block);
GroundwaterData* get_state_data(const char* state, int* count);
GroundwaterData* get_critical_areas(int* count);

// Memory management
void free_query_result(QueryResult* result);

// Sample data access
extern GroundwaterData sample_data[];
extern int sample_data_count;

#endif // DATABASE_H