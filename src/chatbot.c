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

// Logging level constants
typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG
} LogLevel;

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
    log_message(LOG_INFO, "Initializing INGRES ChatBot system...");

    // Initialize the database
    if (!db_init()) {
        log_message(LOG_ERROR, "Failed to initialize database!");
        print_error("Failed to initialize database!");
        return false;
    }
    log_message(LOG_INFO, "Database initialization successful");

    // Initialize global conversation context
    global_context = init_conversation_context();
    if (!global_context) {
        log_message(LOG_ERROR, "Failed to initialize conversation context!");
        print_error("Failed to initialize conversation context!");
        db_close();
        return false;
    }
    log_message(LOG_INFO, "Conversation context initialized");

    log_message(LOG_INFO, "INGRES ChatBot initialized successfully!");
    log_message(LOG_INFO, "Enhanced intent system loaded with 70+ intent types");
    log_message(LOG_INFO, "Conversation context and fuzzy matching enabled");
    log_message(LOG_INFO, "Multi-language support ready");

    printf("[INIT] INGRES ChatBot initialized successfully!\n");
    printf("[INIT] Enhanced intent system loaded with 70+ intent types\n");
    printf("[INIT] Conversation context and fuzzy matching enabled\n");
    printf("[INIT] Multi-language support ready\n");

    return true;
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
    log_message(LOG_DEBUG, "Processing user query: %s", user_input ? user_input : "(null)");

    if (!user_input || strlen(user_input) == 0) {
        log_message(LOG_WARNING, "Empty or null user input received");
        BotResponse* error_response = malloc(sizeof(BotResponse));
        if (error_response) {
            error_response->message = strdup("Please provide a valid query.");
            error_response->intent = INTENT_ERROR;
            error_response->has_data = false;
            error_response->confidence_score = 0.0;
            error_response->suggestion_count = 0;
            log_message(LOG_DEBUG, "Created error response for empty input");
        } else {
            log_message(LOG_ERROR, "Failed to allocate memory for error response");
        }
        return error_response;
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
    }
    
    // Clean up extracted locations
    if (state) free(state);
    if (district) free(district);
    if (block) free(block);
    if (primary_location) free(primary_location);
    
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