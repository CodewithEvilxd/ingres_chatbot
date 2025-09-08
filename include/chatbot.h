#ifndef CHATBOT_H
#define CHATBOT_H

#include <stdbool.h>
#include <stdatomic.h>
#include "utils.h"
#include "database.h"

// Thread safety for concurrent requests
typedef struct {
    atomic_int active_requests;
    atomic_flag lock;
} ThreadSafeCounter;

// Configuration constants - increased for better performance
#define MAX_INPUT_LENGTH 2048
#define MAX_RESPONSE_LENGTH 4096
#define MAX_CONCURRENT_REQUESTS 100
#define CACHE_SIZE 1000
#define SESSION_TIMEOUT_SECONDS 3600

// Modern C features and thread safety
#define CHATBOT_VERSION "2.1.0-enhanced"
#define CHATBOT_API_VERSION "v2"

// Enhanced error codes for better error handling
typedef enum {
    CHATBOT_SUCCESS = 0,
    CHATBOT_ERROR_MEMORY_ALLOCATION,
    CHATBOT_ERROR_INVALID_INPUT,
    CHATBOT_ERROR_DATABASE_CONNECTION,
    CHATBOT_ERROR_INTENT_CLASSIFICATION,
    CHATBOT_ERROR_RESPONSE_GENERATION,
    CHATBOT_ERROR_THREAD_SAFETY,
    CHATBOT_ERROR_CACHE_FULL,
    CHATBOT_ERROR_SESSION_EXPIRED,
    CHATBOT_ERROR_WEB_INTEGRATION
} ChatbotError;

// Web integration structures
typedef struct {
    char endpoint[256];
    char method[16];
    char content_type[64];
    int timeout_ms;
    bool enable_cors;
} WebEndpoint;

typedef struct {
    char key[128];
    BotResponse value;
    time_t timestamp;
    int access_count;
} CacheEntry;

typedef struct {
    CacheEntry entries[CACHE_SIZE];
    int size;
    ThreadSafeCounter counter;
} ResponseCache;

/**
 * @brief Defines the type of user intent identified by the chatbot.
 *
 * This enum is used to categorize the user's query to determine the
 * appropriate action and response.
 */
typedef enum {
    // Basic Intents
    INTENT_UNKNOWN,
    INTENT_GREETING,
    INTENT_GOODBYE,
    INTENT_HELP,
    INTENT_STATUS,
    INTENT_ERROR,
    
    // Location & Data Queries
    INTENT_QUERY_LOCATION,          // "groundwater in Punjab"
    INTENT_QUERY_DISTRICT,          // "show me Amritsar district data"
    INTENT_QUERY_BLOCK,             // "data for Ajnala block"
    INTENT_QUERY_MULTIPLE_LOCATIONS, // "show data for Punjab, Haryana, Gujarat"
    INTENT_QUERY_NEARBY,            // "areas near Delhi"
    INTENT_QUERY_COORDINATES,       // "groundwater at 28.6139°N, 77.2090°E"
    
    // Category & Classification
    INTENT_QUERY_CATEGORY,          // "show over-exploited areas"
    INTENT_CRITICAL_AREAS,          // "which areas are critical"
    INTENT_SAFE_AREAS,              // "safe groundwater zones"
    INTENT_SEMI_CRITICAL_AREAS,     // "semi-critical regions"
    INTENT_OVER_EXPLOITED_AREAS,    // "over-exploited blocks"
    INTENT_CATEGORY_EXPLANATION,    // "what does critical mean"
    INTENT_CLASSIFICATION_CRITERIA, // "how are areas classified"
    
    // Comparisons & Analysis
    INTENT_COMPARE_LOCATIONS,       // "compare Punjab and Haryana"
    INTENT_COMPARE_CATEGORIES,      // "difference between safe and critical"
    INTENT_COMPARE_YEARS,           // "compare 2022 vs 2023 data"
    INTENT_RANK_LOCATIONS,          // "rank states by groundwater depletion"
    INTENT_BEST_WORST_AREAS,        // "best and worst performing states"
    
    // Temporal & Trends
    INTENT_HISTORICAL_TREND,        // "trend for Maharashtra"
    INTENT_YEARLY_COMPARISON,       // "2023 vs 2022 comparison"
    INTENT_SEASONAL_ANALYSIS,       // "monsoon impact analysis"
    INTENT_FUTURE_PREDICTION,       // "predict future trends"
    INTENT_RATE_OF_CHANGE,          // "how fast is depletion happening"
    
    // Crisis & Emergency
    INTENT_WATER_CRISIS,            // "areas facing water crisis"
    INTENT_EMERGENCY_AREAS,         // "emergency intervention needed"
    INTENT_DROUGHT_IMPACT,          // "drought affected regions"
    INTENT_FLOOD_IMPACT,            // "flood impact on groundwater"
    INTENT_CLIMATE_IMPACT,          // "climate change effects"
    
    // Environmental Factors
    INTENT_RAINFALL_CORRELATION,    // "rainfall vs groundwater relation"
    INTENT_MONSOON_ANALYSIS,        // "monsoon impact on recharge"
    INTENT_TEMPERATURE_IMPACT,      // "temperature effects on groundwater"
    INTENT_SOIL_TYPE_ANALYSIS,      // "soil type and groundwater relation"
    INTENT_GEOLOGY_IMPACT,          // "geological factors affecting groundwater"
    
    // Policy & Management
    INTENT_POLICY_SUGGESTION,       // "policy recommendations"
    INTENT_CONSERVATION_METHODS,    // "water conservation techniques"
    INTENT_RECHARGE_METHODS,        // "artificial recharge methods"
    INTENT_REGULATION_INFO,         // "groundwater regulations"
    INTENT_GOVERNMENT_SCHEMES,      // "government water schemes"
    INTENT_SUCCESS_STORIES,         // "successful conservation examples"
    
    // Technical & Scientific
    INTENT_TECHNICAL_EXPLANATION,   // "explain stage of extraction"
    INTENT_CALCULATION_METHOD,      // "how is recharge calculated"
    INTENT_DATA_SOURCE,             // "where does this data come from"
    INTENT_METHODOLOGY,             // "CGWB assessment methodology"
    INTENT_UNITS_EXPLANATION,       // "what is MCM unit"
    
    // Agricultural & Industrial
    INTENT_AGRICULTURE_IMPACT,      // "agriculture effect on groundwater"
    INTENT_CROP_RECOMMENDATIONS,    // "suitable crops for water-scarce areas"
    INTENT_INDUSTRIAL_IMPACT,       // "industrial water usage impact"
    INTENT_IRRIGATION_ANALYSIS,     // "irrigation efficiency analysis"
    INTENT_FARMING_PRACTICES,       // "sustainable farming practices"
    
    // Urban & Infrastructure
    INTENT_URBAN_GROUNDWATER,       // "urban groundwater challenges"
    INTENT_CITY_WATER_STATUS,       // "city-wise water availability"
    INTENT_INFRASTRUCTURE_NEEDS,    // "water infrastructure requirements"
    INTENT_POPULATION_IMPACT,       // "population growth impact"
    
    // Economic & Social
    INTENT_ECONOMIC_IMPACT,         // "economic cost of water crisis"
    INTENT_SOCIAL_IMPACT,           // "social effects of water scarcity"
    INTENT_LIVELIHOOD_IMPACT,       // "impact on farmer livelihoods"
    INTENT_MIGRATION_ANALYSIS,      // "water-induced migration patterns"
    
    // Alerts & Notifications
    INTENT_ALERT_SETUP,             // "set alert for Punjab water levels"
    INTENT_THRESHOLD_MONITORING,    // "monitor critical thresholds"
    INTENT_EARLY_WARNING,           // "early warning systems"
    
    // Data Export & Reports
    INTENT_EXPORT_DATA,             // "export Punjab data to CSV"
    INTENT_GENERATE_REPORT,         // "generate detailed report"
    INTENT_SUMMARY_STATISTICS,      // "summary stats for India"
    INTENT_VISUALIZATION_REQUEST,   // "show me charts for trends"
    
    // Multi-language & Accessibility
    INTENT_LANGUAGE_CHANGE,         // "switch to Hindi"
    INTENT_VOICE_QUERY,             // Voice input processing
    INTENT_ACCESSIBILITY_HELP,      // Screen reader support
    
    // Advanced Analytics
    INTENT_CORRELATION_ANALYSIS,    // "correlation between factors"
    INTENT_STATISTICAL_ANALYSIS,    // "statistical significance"
    INTENT_MACHINE_LEARNING_INSIGHTS, // "AI-based predictions"
    INTENT_ANOMALY_DETECTION,       // "detect unusual patterns"
    
    // Integration & API
    INTENT_API_QUERY,               // "API endpoint information"
    INTENT_DATA_INTEGRATION,        // "integrate with external data"
    INTENT_REAL_TIME_DATA,          // "real-time monitoring data"
    
    // Context-Aware Intents
    INTENT_FOLLOW_UP_QUESTION,      // "tell me more about that"
    INTENT_CLARIFICATION_REQUEST,   // "what do you mean by that"
    INTENT_PREVIOUS_CONTEXT,        // "go back to previous topic"
    INTENT_CONVERSATION_SUMMARY     // "summarize our conversation"
} IntentType;

/**
 * @brief Conversation context for maintaining state across interactions
 */
typedef struct {
    char* last_location;        // Last queried location
    char* last_state;           // Last queried state
    char* last_district;        // Last queried district
    IntentType last_intent;     // Previous intent for context
    char* conversation_history[10]; // Last 10 interactions
    int history_count;          // Number of items in history
    bool awaiting_clarification; // Waiting for user clarification
    char* pending_question;     // Question awaiting answer
    time_t session_start;       // Session start time
    int query_count;            // Number of queries in session
} ConversationContext;

/**
 * @brief Enhanced response structure with context and metadata
 */
typedef struct {
    char* message;              // The primary text response for the user.
    IntentType intent;          // The classified intent of the user's query.
    QueryResult* query_result;  // Pointer to database query results, if any.
    bool has_data;              // Flag indicating if query_result is populated.
    float processing_time_ms;   // Time taken to process the query, in milliseconds.
    float confidence_score;     // Confidence in intent classification (0-1)
    char* suggested_actions[5]; // Suggested follow-up actions
    int suggestion_count;       // Number of suggestions
    bool requires_clarification; // Whether response needs user clarification
    char* clarification_question; // Question to ask user for clarification
    ConversationContext* context; // Conversation context
    char* data_sources[3];      // Sources of data used
    int source_count;           // Number of data sources
    bool is_multilingual;       // Whether response supports multiple languages
    char* language_code;        // Language of the response (en, hi, etc.)
} BotResponse;

/**
 * @brief Initializes the chatbot.
 *
 * Loads necessary resources like the database. This must be called before any
 * other chatbot functions.
 *
 * @return true if initialization is successful, false otherwise.
 */
bool chatbot_init(void);

/**
 * @brief Cleans up and frees all resources used by the chatbot.
 *
 * This should be called when the application is shutting down to prevent
 * memory leaks.
 */
void chatbot_cleanup(void);

/**
 * @brief Processes a user's query and returns a structured response.
 *
 * This is the core function for interacting with the chatbot. It takes a raw
 * string from the user, classifies the intent, performs database queries if
 * needed, and constructs a complete response object.
 *
 * @param user_input The raw string input from the user.
 * @return A pointer to a dynamically allocated BotResponse struct. The caller
 *         is responsible for freeing this memory by calling `free_bot_response`.
 */
BotResponse* process_user_query(const char* user_input);

/**
 * @brief Frees the memory allocated for a BotResponse struct.
 *
 * This function safely deallocates the BotResponse and its internal members,
 * including the QueryResult if it exists.
 *
 * @param response A pointer to the BotResponse to be freed.
 */
void free_bot_response(BotResponse* response);

/**
 * @brief Advanced intent classification with context awareness
 *
 * @param user_input The raw string input from the user.
 * @param context Previous conversation context (can be NULL).
 * @param confidence Pointer to store confidence score (0-1).
 * @return The classified intent type.
 */
IntentType classify_intent_advanced(const char* user_input, ConversationContext* context, float* confidence);

/**
 * @brief Legacy simple intent classification (for backward compatibility)
 *
 * @param user_input The raw string input from the user.
 * @return The classified intent type.
 */
IntentType classify_intent(const char* user_input);

/**
 * @brief Enhanced response generation with context and personalization
 *
 * @param intent The classified intent type.
 * @param user_input Original user input for context.
 * @param context Conversation context.
 * @param location The location context (can be NULL).
 * @param query_details Additional query details (can be NULL).
 * @return A dynamically allocated enhanced BotResponse.
 */
BotResponse* generate_enhanced_response(IntentType intent, const char* user_input, 
                                      ConversationContext* context, const char* location, 
                                      const char* query_details);

/**
 * @brief Legacy response generation (for backward compatibility)
 *
 * @param intent The classified intent type.
 * @param location The location context (can be NULL).
 * @param query_details Additional query details (can be NULL).
 * @return A dynamically allocated response string.
 */
char* generate_response(IntentType intent, const char* location, const char* query_details);

/**
 * @brief Extract location entities from user input
 *
 * @param user_input The raw string input from the user.
 * @param state Pointer to store extracted state (allocated by function).
 * @param district Pointer to store extracted district (allocated by function).
 * @param block Pointer to store extracted block (allocated by function).
 * @return Number of locations extracted.
 */
int extract_locations(const char* user_input, char** state, char** district, char** block);

/**
 * @brief Calculate fuzzy string similarity (Levenshtein distance based)
 *
 * @param str1 First string.
 * @param str2 Second string.
 * @return Similarity score (0-1, where 1 is exact match).
 */
float calculate_similarity(const char* str1, const char* str2);

/**
 * @brief Initialize conversation context
 *
 * @return Pointer to initialized conversation context.
 */
ConversationContext* init_conversation_context(void);

/**
 * @brief Update conversation context with new interaction
 *
 * @param context Conversation context to update.
 * @param user_input User's input.
 * @param intent Classified intent.
 * @param location Extracted location (can be NULL).
 */
void update_conversation_context(ConversationContext* context, const char* user_input, 
                               IntentType intent, const char* location);

/**
 * @brief Free conversation context memory
 *
 * @param context Conversation context to free.
 */
void free_conversation_context(ConversationContext* context);

// ============================================================================
// ENHANCED MODERN FEATURES
// ============================================================================

/**
 * @brief Initialize enhanced chatbot with modern features
 *
 * @param config Configuration structure for enhanced features
 * @return true if initialization successful
 */
bool chatbot_init_enhanced(void* config);

/**
 * @brief Get last error code and message
 *
 * @param error_code Pointer to store error code
 * @return Error message string
 */
const char* chatbot_get_last_error(ChatbotError* error_code);

/**
 * @brief Process user query with caching and performance optimization
 *
 * @param user_input User input string
 * @param session_id Session identifier for caching
 * @return Enhanced BotResponse with caching and performance metrics
 */
BotResponse* process_user_query_enhanced(const char* user_input, const char* session_id);

/**
 * @brief Initialize response cache for performance optimization
 *
 * @return true if cache initialization successful
 */
bool init_response_cache(void);

/**
 * @brief Get cached response if available
 *
 * @param cache_key Cache key to lookup
 * @return Cached BotResponse or NULL if not found
 */
BotResponse* get_cached_response(const char* cache_key);

/**
 * @brief Store response in cache
 *
 * @param cache_key Cache key
 * @param response Response to cache
 * @return true if caching successful
 */
bool cache_response(const char* cache_key, const BotResponse* response);

/**
 * @brief Clear expired cache entries
 *
 * @return Number of entries cleared
 */
int clear_expired_cache(void);

/**
 * @brief Get cache statistics
 *
 * @param hit_rate Pointer to store cache hit rate
 * @param size Pointer to store current cache size
 */
void get_cache_stats(float* hit_rate, int* size);

/**
 * @brief Initialize web integration endpoints
 *
 * @return true if web integration initialized successfully
 */
bool init_web_integration(void);

/**
 * @brief Process web request and return JSON response
 *
 * @param request_json JSON request string
 * @param response_json Pointer to response JSON string (allocated by function)
 * @return true if processing successful
 */
bool process_web_request(const char* request_json, char** response_json);

/**
 * @brief Get system health and performance metrics
 *
 * @param metrics_json Pointer to metrics JSON string (allocated by function)
 * @return true if metrics retrieval successful
 */
bool get_system_health(char** metrics_json);

/**
 * @brief Thread-safe counter for concurrent request tracking
 */
extern ThreadSafeCounter request_counter;

/**
 * @brief Global response cache instance
 */
extern ResponseCache* global_cache;

#endif // CHATBOT_H