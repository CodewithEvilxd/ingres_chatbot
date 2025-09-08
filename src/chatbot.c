#include "chatbot.h"
#include "utils.h"
#include "database.h"
#include "intent_patterns.h"
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Enhanced logging with modern C features
typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG
} LogLevel;

// Enhanced error handling
static ChatbotError last_error = CHATBOT_SUCCESS;
static char last_error_message[256] = "";

// Global instances for enhanced features
ThreadSafeCounter request_counter = {0};
ResponseCache* global_cache = NULL;
static bool enhanced_features_initialized = false;

// External logging function declaration
extern void log_message(LogLevel level, const char* format, ...);

// Forward declarations for enhanced functions
extern IntentType classify_intent_advanced(const char* user_input, ConversationContext* context, float* confidence);
extern BotResponse* generate_enhanced_response(IntentType intent, const char* user_input,
                                              ConversationContext* context, const char* location,
                                              const char* query_details);
extern int extract_locations(const char* user_input, char** state, char** district, char** block);
extern ConversationContext* init_conversation_context(void);
extern void update_conversation_context(ConversationContext* context, const char* user_input,
                                       IntentType intent, const char* location);
extern void free_conversation_context(ConversationContext* context);
extern void free_enhanced_bot_response(BotResponse* response);

// Global conversation context for session management
static ConversationContext* global_context = NULL;

bool chatbot_init(void) {
    return chatbot_init_enhanced(NULL);
}

bool chatbot_init_enhanced(void* config) {
    log_message(LOG_INFO, "Initializing INGRES ChatBot system with enhanced features...");

    // Reset error state
    last_error = CHATBOT_SUCCESS;
    last_error_message[0] = '\0';

    // Initialize the database
    if (!db_init()) {
        last_error = CHATBOT_ERROR_DATABASE_CONNECTION;
        snprintf(last_error_message, sizeof(last_error_message),
                "Failed to initialize database connection");
        log_message(LOG_ERROR, "%s", last_error_message);
        print_error("Failed to initialize database!");
        return false;
    }
    log_message(LOG_INFO, "Database initialization successful");

    // Initialize global conversation context
    global_context = init_conversation_context();
    if (!global_context) {
        last_error = CHATBOT_ERROR_MEMORY_ALLOCATION;
        snprintf(last_error_message, sizeof(last_error_message),
                "Failed to initialize conversation context - memory allocation error");
        log_message(LOG_ERROR, "%s", last_error_message);
        print_error("Failed to initialize conversation context!");
        db_close();
        return false;
    }
    log_message(LOG_INFO, "Conversation context initialized");

    // Initialize response cache
    if (!init_response_cache()) {
        last_error = CHATBOT_ERROR_MEMORY_ALLOCATION;
        snprintf(last_error_message, sizeof(last_error_message),
                "Failed to initialize response cache");
        log_message(LOG_WARNING, "%s - continuing without cache", last_error_message);
        // Don't fail initialization for cache issues
    }

    // Initialize web integration
    if (!init_web_integration()) {
        last_error = CHATBOT_ERROR_WEB_INTEGRATION;
        snprintf(last_error_message, sizeof(last_error_message),
                "Failed to initialize web integration");
        log_message(LOG_WARNING, "%s - web features may be limited", last_error_message);
        // Don't fail initialization for web integration issues
    }

    enhanced_features_initialized = true;

    log_message(LOG_INFO, "INGRES ChatBot initialized successfully!");
    log_message(LOG_INFO, "Enhanced features: Caching, Web Integration, Thread Safety");
    log_message(LOG_INFO, "Enhanced intent system loaded with 70+ intent types");
    log_message(LOG_INFO, "Conversation context and fuzzy matching enabled");
    log_message(LOG_INFO, "Multi-language support ready");

    printf("[INIT] INGRES ChatBot %s initialized successfully!\n", CHATBOT_VERSION);
    printf("[INIT] Enhanced features: Caching, Web API, Thread Safety\n");
    printf("[INIT] Enhanced intent system loaded with 70+ intent types\n");
    printf("[INIT] Conversation context and fuzzy matching enabled\n");
    printf("[INIT] Multi-language support ready\n");

    return true;
}

const char* chatbot_get_last_error(ChatbotError* error_code) {
    if (error_code) {
        *error_code = last_error;
    }
    return last_error_message[0] != '\0' ? last_error_message : "No error";
}

void chatbot_cleanup(void) {
    // Clean up conversation context
    if (global_context) {
        free_conversation_context(global_context);
        global_context = NULL;
    }
    
    // Close the database
    db_close();
    
    printf("[CLEANUP] INGRES ChatBot cleanup completed\n");
}

// Legacy function for backward compatibility
IntentType classify_intent(const char* user_input) {
    float confidence;
    return classify_intent_advanced(user_input, global_context, &confidence);
}

// Enhanced main processing function
BotResponse* process_user_query(const char* user_input) {
    return process_user_query_enhanced(user_input, NULL);
}

BotResponse* process_user_query_enhanced(const char* user_input, const char* session_id) {
    log_message(LOG_DEBUG, "Processing user query: %s", user_input ? user_input : "(null)");

    // Thread-safe request counting
    atomic_fetch_add(&request_counter.active_requests, 1);

    if (!user_input || strlen(user_input) == 0) {
        last_error = CHATBOT_ERROR_INVALID_INPUT;
        snprintf(last_error_message, sizeof(last_error_message),
                "Empty or null user input received");

        log_message(LOG_WARNING, "%s", last_error_message);

        BotResponse* error_response = malloc(sizeof(BotResponse));
        if (error_response) {
            error_response->message = strdup("Please provide a valid query.");
            error_response->intent = INTENT_ERROR;
            error_response->has_data = false;
            error_response->confidence_score = 0.0;
            error_response->suggestion_count = 0;
            log_message(LOG_DEBUG, "Created error response for empty input");
        } else {
            last_error = CHATBOT_ERROR_MEMORY_ALLOCATION;
            log_message(LOG_ERROR, "Failed to allocate memory for error response");
        }

        atomic_fetch_sub(&request_counter.active_requests, 1);
        return error_response;
    }

    // Check cache first if session_id provided
    if (session_id && global_cache) {
        char cache_key[256];
        snprintf(cache_key, sizeof(cache_key), "%s_%s", session_id, user_input);
        BotResponse* cached_response = get_cached_response(cache_key);
        if (cached_response) {
            log_message(LOG_DEBUG, "Cache hit for query: %s", user_input);
            atomic_fetch_sub(&request_counter.active_requests, 1);
            return cached_response;
        }
    }

    clock_t start_time = clock();

    // Extract locations from user input
    char* state = NULL;
    char* district = NULL;
    char* block = NULL;
    int locations_found = extract_locations(user_input, &state, &district, &block);

    // Classify intent with enhanced system
    float confidence;
    IntentType intent = classify_intent_advanced(user_input, global_context, &confidence);

    // Determine primary location for context
    char* primary_location = NULL;
    if (state) {
        primary_location = strdup(state);
    } else if (district) {
        primary_location = strdup(district);
    } else if (global_context && global_context->last_location) {
        primary_location = strdup(global_context->last_location);
    }

    // Generate enhanced response
    BotResponse* response = generate_enhanced_response(intent, user_input, global_context,
                                                      primary_location, user_input);

    if (response) {
        // Update confidence score
        response->confidence_score = confidence;

        // Calculate processing time
        clock_t end_time = clock();
        response->processing_time_ms = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000.0;

        // Update conversation context
        update_conversation_context(global_context, user_input, intent, primary_location);

        // Add clarification if confidence is low
        if (confidence < 0.5) {
            response->requires_clarification = true;
            response->clarification_question = strdup(
                "I'm not entirely sure I understood your query correctly. "
                "Could you please rephrase or provide more specific details?"
            );
        }

        // Cache the response if session_id provided
        if (session_id && global_cache && response->confidence_score > 0.7) {
            char cache_key[256];
            snprintf(cache_key, sizeof(cache_key), "%s_%s", session_id, user_input);
            cache_response(cache_key, response);
        }
    } else {
        last_error = CHATBOT_ERROR_RESPONSE_GENERATION;
        snprintf(last_error_message, sizeof(last_error_message),
                "Failed to generate response for query: %s", user_input);
    }

    // Clean up extracted locations
    if (state) free(state);
    if (district) free(district);
    if (block) free(block);
    if (primary_location) free(primary_location);

    atomic_fetch_sub(&request_counter.active_requests, 1);
    return response;
}
// Simplified process_user_input for testing
BotResponse* process_user_input(const char* user_input) {
    BotResponse* response = malloc(sizeof(BotResponse));
    if (!response) return NULL;
    
    IntentType intent = classify_intent(user_input);
    response->intent = intent;
    response->message = generate_response(intent, "TestLocation", user_input);
    response->query_result = NULL;
    response->has_data = false;
    response->processing_time_ms = 0.0f;
    
    return response;
}

char* generate_response(IntentType intent, const char* location, const char* query_details) {
    char* response = malloc(MAX_RESPONSE_LENGTH);
    if (!response) return NULL;
    
    switch (intent) {
        case INTENT_GREETING:
            snprintf(response, MAX_RESPONSE_LENGTH, 
                "Namaste! I'm your INGRES groundwater assistant. I can help you with:\n"
                "* Groundwater data for any location\n"
                "* Critical and over-exploited areas\n"
                "* Historical trends and comparisons\n"
                "* Policy recommendations\n\n"
                "Try asking: 'Show me Punjab groundwater data' or 'Which areas are critical?'");
            break;
            
        case INTENT_GOODBYE:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "Thank you for using INGRES ChatBot! Stay informed about groundwater resources.");
            break;
            
        case INTENT_HELP:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "INGRES ChatBot Help:\n\n"
                "[LOC] Location Queries: 'Show me [state/district] data'\n"
                "[CRIT] Critical Areas: 'Show critical areas' or 'Over-exploited regions'\n"
                "[COMP] Comparisons: 'Compare Punjab and Haryana'\n"
                "[TREND] Trends: 'Historical trend for Maharashtra'\n"
                "[CRISIS] Water Crisis: 'Areas facing water shortage'\n"
                "[RAIN] Rainfall Impact: 'How does rainfall affect groundwater?'\n"
                "[POLICY] Policy Help: 'Policy suggestions for [location]'\n\n"
                "Example: 'What is the groundwater status of Amritsar?'");
            break;
            
        case INTENT_QUERY_LOCATION:
            if (location && strlen(location) > 0) {
                // This will be enhanced when we connect to actual database
                snprintf(response, MAX_RESPONSE_LENGTH,
                    "[SEARCH] Searching groundwater data for %s...\n\n"
                    "Based on latest CGWB assessment:\n"
                    "* Location: %s\n"
                    "* Assessment Year: 2023\n"
                    "* Status: [Fetching from database...]\n"
                    "* Stage of Extraction: [Calculating...]\n\n"
                    "Note: Connecting to INGRES database for real-time data...", 
                    location, location);
            } else {
                snprintf(response, MAX_RESPONSE_LENGTH,
                    "Please specify a location. Example:\n"
                    "* 'Show me Punjab groundwater data'\n"
                    "* 'Groundwater status of Amritsar'\n"
                    "* 'Data for Maharashtra'");
            }
            break;
            
        case INTENT_CRITICAL_AREAS:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[ALERT] CRITICAL GROUNDWATER AREAS (Latest Assessment):\n\n"
                "[OVER-EXPLOITED] (>100%% extraction):\n"
                "* Punjab: Amritsar, Ludhiana, Bathinda\n"
                "* Haryana: Sirsa, Hisar\n"
                "* Gujarat: Ahmedabad City\n"
                "* Tamil Nadu: Chennai\n\n"
                "[CRITICAL] (90-100%% extraction):\n"
                "* Maharashtra: Pune, Aurangabad\n"
                "* Karnataka: Bangalore Urban\n"
                "* Rajasthan: Alwar\n\n"
                "NOTE: These areas need immediate water conservation measures!");
            break;
            
        case INTENT_QUERY_CATEGORY:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[CATEGORY] GROUNDWATER CLASSIFICATION SYSTEM:\n\n"
                "CGWB CATEGORIES (Based on Stage of Extraction):\n\n"
                "[SAFE] <70%% extraction:\n"
                "* Sustainable groundwater usage\n"
                "* No restrictions on new bore wells\n"
                "* Examples: Jaipur, Mysore, Kolkata\n\n"
                "[SEMI-CRITICAL] 70-90%% extraction:\n"
                "* Moderate stress on groundwater\n"
                "* Regulated development needed\n"
                "* Examples: Kurukshetra, Nashik, Coimbatore\n\n"
                "[CRITICAL] 90-100%% extraction:\n"
                "* High stress, immediate action needed\n"
                "* Strict regulations required\n"
                "* Examples: Pune, Bangalore, Alwar\n\n"
                "[OVER-EXPLOITED] >100%% extraction:\n"
                "* Groundwater mining occurring\n"
                "* Ban on new extractions\n"
                "* Examples: Amritsar, Ludhiana, Chennai");
            break;
            
        case INTENT_SAFE_AREAS:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[SAFE] SAFE GROUNDWATER AREAS (<70%% extraction):\n\n"
                "[SAFE REGIONS]:\n"
                "* Rajasthan: Jaipur, Bikaner\n"
                "* Gujarat: Vadodara\n"
                "* Maharashtra: Mumbai Suburban\n"
                "* Tamil Nadu: Madurai\n"
                "* Karnataka: Mysore\n"
                "* West Bengal: Kolkata, Howrah\n"
                "* Andhra Pradesh: Visakhapatnam\n\n"
                "NOTE: These areas have sustainable groundwater usage patterns.");
            break;
            
        case INTENT_WATER_CRISIS:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[CRISIS] WATER CRISIS ALERT AREAS:\n\n"
                "IMMEDIATE ATTENTION NEEDED:\n"
                "* Punjab (Central): 78%% blocks over-exploited\n"
                "* Haryana (Southwest): Rapid depletion rate\n"
                "* Tamil Nadu (Chennai): Urban water stress\n"
                "* Karnataka (Bangalore): IT sector impact\n\n"
                "CRISIS INDICATORS:\n"
                "* Declining water table: 0.5-2m annually\n"
                "* Industrial over-extraction\n"
                "* Intensive agriculture in arid regions\n\n"
                "URGENT ACTIONS: Rainwater harvesting, drip irrigation, policy enforcement");
            break;
            
        case INTENT_RAINFALL_CORRELATION:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[RAINFALL] RAINFALL-GROUNDWATER CORRELATION:\n\n"
                "MONSOON IMPACT:\n"
                "* Good Monsoon (>110%% normal): +15-25%% recharge\n"
                "* Normal Monsoon (90-110%%): Stable recharge\n"
                "* Poor Monsoon (<90%%): -20-40%% recharge\n\n"
                "REGIONAL PATTERNS:\n"
                "* Western Ghats: High recharge efficiency (60-80%%)\n"
                "* Gangetic Plains: Moderate efficiency (40-60%%)\n"
                "* Arid Regions: Low efficiency (10-30%%)\n\n"
                "NOTE: Climate change is affecting traditional recharge patterns!");
            break;
            
        case INTENT_POLICY_SUGGESTION:
            if (location && strlen(location) > 0) {
                snprintf(response, MAX_RESPONSE_LENGTH,
                    "[POLICY] POLICY RECOMMENDATIONS FOR %s:\n\n"
                    "IMMEDIATE MEASURES:\n"
                    "* Mandatory rainwater harvesting for buildings >300 sq.m\n"
                    "* Groundwater extraction permits with annual limits\n"
                    "* Subsidies for drip irrigation systems\n\n"
                    "MEDIUM-TERM STRATEGIES:\n"
                    "* Crop diversification from water-intensive crops\n"
                    "* Industrial water recycling mandates\n"
                    "* Community-based water management\n\n"
                    "MONITORING:\n"
                    "* Real-time groundwater level monitoring\n"
                    "* Annual assessment updates\n"
                    "* Public awareness campaigns", location);
            } else {
                snprintf(response, MAX_RESPONSE_LENGTH,
                    "[POLICY] GENERAL GROUNDWATER POLICY FRAMEWORK:\n\n"
                    "NATIONAL LEVEL:\n"
                    "* National Water Policy implementation\n"
                    "* CGWB guidelines enforcement\n"
                    "* Inter-state water sharing agreements\n\n"
                    "STATE LEVEL:\n"
                    "* Groundwater regulation acts\n"
                    "* Water conservation incentives\n"
                    "* Agricultural policy reforms\n\n"
                    "Specify a location for targeted recommendations!");
            }
            break;
            
        case INTENT_COMPARE_LOCATIONS:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[COMPARE] GROUNDWATER COMPARISON:\n\n"
                "PUNJAB vs HARYANA (Example Comparison):\n\n"
                "PUNJAB:\n"
                "* Over-exploited: 78%% of blocks\n"
                "* Stage of Extraction: >150%% in many areas\n"
                "* Main Issue: Intensive rice-wheat cultivation\n"
                "* Water Table: Declining 0.5-1m annually\n\n"
                "HARYANA:\n"
                "* Over-exploited: 45%% of blocks\n"
                "* Stage of Extraction: 90-120%% in critical areas\n"
                "* Main Issue: Industrial + agricultural demand\n"
                "* Water Table: Mixed trends by region\n\n"
                "RECOMMENDATION: Both states need urgent water conservation measures.\n"
                "Specify exact locations for detailed comparison.");
            break;
            
        case INTENT_HISTORICAL_TREND:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[TREND] HISTORICAL GROUNDWATER TRENDS:\n\n"
                "NATIONAL TRENDS (2017-2023):\n\n"
                "DECLINING AREAS:\n"
                "* Punjab: Water table dropped 2-3m\n"
                "* Haryana: 15%% increase in over-exploited blocks\n"
                "* Tamil Nadu: Chennai crisis worsened\n"
                "* Karnataka: Bangalore stress increased\n\n"
                "IMPROVING AREAS:\n"
                "* Rajasthan: Better monsoon management\n"
                "* Gujarat: Successful water conservation\n"
                "* Maharashtra: Watershed programs effective\n\n"
                "KEY FACTORS:\n"
                "* Climate change impact\n"
                "* Policy interventions\n"
                "* Agricultural practices\n\n"
                "Specify a location for detailed trend analysis.");
            break;
            
        case INTENT_STATUS:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[STATUS] INGRES CHATBOT STATUS:\n\n"
                "[OK] System: Online and operational\n"
                "[OK] Database: Connected to sample data\n"
                "[OK] Coverage: Pan-India groundwater data\n"
                "[OK] Languages: English (Hindi support coming soon)\n"
                "[OK] Last Update: 2023 CGWB Assessment\n\n"
                "CAPABILITIES:\n"
                "* 6000+ assessment units covered\n"
                "* Real-time query processing\n"
                "* Historical trend analysis\n"
                "* Policy recommendation engine");
            break;
            
        case INTENT_UNKNOWN:
        default:
            snprintf(response, MAX_RESPONSE_LENGTH,
                "[HELP] I didn't quite understand that. I can help with:\n\n"
                "* 'Show me [location] groundwater data'\n"
                "* 'Which areas are critical?'\n"
                "* 'Compare [state1] and [state2]'\n"
                "* 'Policy suggestions for [location]'\n"
                "* 'Areas facing water crisis'\n\n"
                "Try rephrasing your question or type 'help' for more options.");
            break;
    }
    
    return response;
}


void free_bot_response(BotResponse* response) {
    if (!response) return;

    if (response->message) {
        free(response->message);
    }

    if (response->query_result) {
        free_query_result(response->query_result);
    }

    free(response);
}

// ============================================================================
// ENHANCED CACHE IMPLEMENTATION
// ============================================================================

bool init_response_cache(void) {
    if (global_cache) {
        return true; // Already initialized
    }

    global_cache = calloc(1, sizeof(ResponseCache));
    if (!global_cache) {
        last_error = CHATBOT_ERROR_MEMORY_ALLOCATION;
        snprintf(last_error_message, sizeof(last_error_message),
                "Failed to allocate memory for response cache");
        return false;
    }

    // Initialize thread-safe counter
    atomic_init(&global_cache->counter.active_requests, 0);
    atomic_flag_clear(&global_cache->counter.lock);

    global_cache->size = 0;

    log_message(LOG_INFO, "Response cache initialized with capacity: %d", CACHE_SIZE);
    return true;
}

BotResponse* get_cached_response(const char* cache_key) {
    if (!global_cache || !cache_key) return NULL;

    // Simple linear search for now (could be optimized with hash table)
    for (int i = 0; i < global_cache->size; i++) {
        if (strcmp(global_cache->entries[i].key, cache_key) == 0) {
            // Check if entry is not expired
            if (time(NULL) - global_cache->entries[i].timestamp < SESSION_TIMEOUT_SECONDS) {
                global_cache->entries[i].access_count++;
                log_message(LOG_DEBUG, "Cache hit for key: %s", cache_key);

                // Return a copy of the cached response
                BotResponse* cached = malloc(sizeof(BotResponse));
                if (cached) {
                    memcpy(cached, &global_cache->entries[i].value, sizeof(BotResponse));
                    // Deep copy strings
                    if (cached->message) {
                        cached->message = strdup(cached->message);
                    }
                    if (cached->clarification_question) {
                        cached->clarification_question = strdup(cached->clarification_question);
                    }
                    if (cached->language_code) {
                        cached->language_code = strdup(cached->language_code);
                    }
                    // Deep copy suggestions
                    for (int j = 0; j < cached->suggestion_count && j < 5; j++) {
                        if (cached->suggested_actions[j]) {
                            cached->suggested_actions[j] = strdup(cached->suggested_actions[j]);
                        }
                    }
                    // Deep copy data sources
                    for (int j = 0; j < cached->source_count && j < 3; j++) {
                        if (cached->data_sources[j]) {
                            cached->data_sources[j] = strdup(cached->data_sources[j]);
                        }
                    }
                }
                return cached;
            } else {
                // Remove expired entry
                memmove(&global_cache->entries[i], &global_cache->entries[i + 1],
                       (global_cache->size - i - 1) * sizeof(CacheEntry));
                global_cache->size--;
                i--; // Adjust index after removal
            }
        }
    }

    return NULL;
}

bool cache_response(const char* cache_key, const BotResponse* response) {
    if (!global_cache || !cache_key || !response) return false;

    // Check if cache is full
    if (global_cache->size >= CACHE_SIZE) {
        // Remove oldest entry (simple LRU approximation)
        memmove(&global_cache->entries[0], &global_cache->entries[1],
               (CACHE_SIZE - 1) * sizeof(CacheEntry));
        global_cache->size--;
    }

    // Add new entry
    CacheEntry* entry = &global_cache->entries[global_cache->size];
    strncpy(entry->key, cache_key, sizeof(entry->key) - 1);
    entry->key[sizeof(entry->key) - 1] = '\0';

    // Shallow copy of response (strings will be duplicated when retrieved)
    memcpy(&entry->value, response, sizeof(BotResponse));
    entry->timestamp = time(NULL);
    entry->access_count = 1;

    global_cache->size++;

    log_message(LOG_DEBUG, "Cached response for key: %s", cache_key);
    return true;
}

int clear_expired_cache(void) {
    if (!global_cache) return 0;

    int cleared = 0;
    time_t current_time = time(NULL);

    for (int i = 0; i < global_cache->size; ) {
        if (current_time - global_cache->entries[i].timestamp >= SESSION_TIMEOUT_SECONDS) {
            // Remove expired entry
            memmove(&global_cache->entries[i], &global_cache->entries[i + 1],
                   (global_cache->size - i - 1) * sizeof(CacheEntry));
            global_cache->size--;
            cleared++;
        } else {
            i++;
        }
    }

    if (cleared > 0) {
        log_message(LOG_INFO, "Cleared %d expired cache entries", cleared);
    }

    return cleared;
}

void get_cache_stats(float* hit_rate, int* size) {
    if (!global_cache) {
        if (hit_rate) *hit_rate = 0.0;
        if (size) *size = 0;
        return;
    }

    if (size) *size = global_cache->size;

    // Calculate hit rate (simplified - would need more sophisticated tracking)
    if (hit_rate) {
        int total_accesses = 0;
        int hits = 0;

        for (int i = 0; i < global_cache->size; i++) {
            total_accesses += global_cache->entries[i].access_count;
            if (global_cache->entries[i].access_count > 1) {
                hits += (global_cache->entries[i].access_count - 1);
            }
        }

        *hit_rate = total_accesses > 0 ? (float)hits / total_accesses : 0.0;
    }
}

// ============================================================================
// WEB INTEGRATION IMPLEMENTATION
// ============================================================================

bool init_web_integration(void) {
    log_message(LOG_INFO, "Initializing web integration for API endpoints");
    // Web integration is primarily handled by the Node.js server
    // This function could be used for any C-side web initialization
    return true;
}

bool process_web_request(const char* request_json, char** response_json) {
    if (!request_json || !response_json) {
        last_error = CHATBOT_ERROR_INVALID_INPUT;
        return false;
    }

    // Simple JSON parsing (in production, use a proper JSON library)
    char* message = NULL;
    char* session_id = NULL;

    // Extract message from JSON (simplified parsing)
    const char* message_start = strstr(request_json, "\"message\":\"");
    if (message_start) {
        message_start += 11; // Skip "message":"
        const char* message_end = strstr(message_start, "\"");
        if (message_end) {
            int message_len = message_end - message_start;
            message = malloc(message_len + 1);
            if (message) {
                strncpy(message, message_start, message_len);
                message[message_len] = '\0';
            }
        }
    }

    // Extract session_id from JSON
    const char* session_start = strstr(request_json, "\"session_id\":\"");
    if (session_start) {
        session_start += 14; // Skip "session_id":"
        const char* session_end = strstr(session_start, "\"");
        if (session_end) {
            int session_len = session_end - session_start;
            session_id = malloc(session_len + 1);
            if (session_id) {
                strncpy(session_id, session_start, session_len);
                session_id[session_len] = '\0';
            }
        }
    }

    if (!message) {
        last_error = CHATBOT_ERROR_INVALID_INPUT;
        free(session_id);
        return false;
    }

    // Process the message
    BotResponse* response = process_user_query_enhanced(message, session_id);

    if (!response) {
        free(message);
        free(session_id);
        return false;
    }

    // Generate JSON response
    char* json_response = malloc(4096); // Allocate sufficient space
    if (!json_response) {
        last_error = CHATBOT_ERROR_MEMORY_ALLOCATION;
        free_bot_response(response);
        free(message);
        free(session_id);
        return false;
    }

    // Create JSON response (simplified - in production use proper JSON library)
    snprintf(json_response, 4096,
             "{"
             "\"message\":\"%s\","
             "\"intent\":%d,"
             "\"confidence\":%.2f,"
             "\"processing_time_ms\":%.2f,"
             "\"has_data\":%s,"
             "\"requires_clarification\":%s,"
             "\"suggestions\":[%s],"
             "\"data_sources\":[%s],"
             "\"groundwater_status\":\"%s\""
             "}",
             response->message ? response->message : "",
             response->intent,
             response->confidence_score,
             response->processing_time_ms,
             response->has_data ? "true" : "false",
             response->requires_clarification ? "true" : "false",
             "\"\"", // Simplified suggestions
             "\"Central Ground Water Board (CGWB)\"", // Simplified data sources
             "normal" // Default groundwater status
    );

    *response_json = json_response;

    // Cleanup
    free_bot_response(response);
    free(message);
    free(session_id);

    return true;
}

bool get_system_health(char** metrics_json) {
    if (!metrics_json) {
        last_error = CHATBOT_ERROR_INVALID_INPUT;
        return false;
    }

    char* json_metrics = malloc(1024);
    if (!json_metrics) {
        last_error = CHATBOT_ERROR_MEMORY_ALLOCATION;
        return false;
    }

    // Get cache stats
    float cache_hit_rate = 0.0;
    int cache_size = 0;
    get_cache_stats(&cache_hit_rate, &cache_size);

    // Create health metrics JSON
    snprintf(json_metrics, 1024,
             "{"
             "\"status\":\"online\","
             "\"version\":\"%s\","
             "\"uptime_seconds\":%ld,"
             "\"active_requests\":%d,"
             "\"cache_size\":%d,"
             "\"cache_hit_rate\":%.2f,"
             "\"enhanced_features\":%s,"
             "\"intent_types\":70,"
             "\"total_states\":28,"
             "\"last_error\":\"%s\""
             "}",
             CHATBOT_VERSION,
             time(NULL) - (global_context ? global_context->session_start : time(NULL)),
             atomic_load(&request_counter.active_requests),
             cache_size,
             cache_hit_rate,
             enhanced_features_initialized ? "true" : "false",
             last_error_message[0] != '\0' ? last_error_message : "none"
    );

    *metrics_json = json_metrics;
    return true;
}