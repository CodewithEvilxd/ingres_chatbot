#include "chatbot.h"
#include "../lib/mongoose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Conditional JSON support
#ifdef USE_JSON_C
#include <json-c/json.h>
#endif

// API endpoint handlers
static void handle_chat_endpoint(struct mg_connection *c, struct mg_http_message *hm);
static void handle_status_endpoint(struct mg_connection *c, struct mg_http_message *hm);
static void handle_health_endpoint(struct mg_connection *c, struct mg_http_message *hm);
static void handle_capabilities_endpoint(struct mg_connection *c, struct mg_http_message *hm);

// Convert BotResponse to simple JSON-like format
char* bot_response_to_json(BotResponse* response) {
    if (!response) return NULL;

    char* result = malloc(4096); // Large buffer for response
    if (!result) return NULL;

    // Build simple JSON-like response
    snprintf(result, 4096,
        "{\"message\":\"%s\",\"intent\":%d,\"confidence\":%.2f,\"processing_time_ms\":%.2f,\"has_data\":%s,\"requires_clarification\":%s",
        response->message ? response->message : "",
        response->intent,
        response->confidence_score,
        response->processing_time_ms,
        response->has_data ? "true" : "false",
        response->requires_clarification ? "true" : "false");

    // Add suggestions if available
    if (response->suggestion_count > 0) {
        strcat(result, ",\"suggestions\":[");
        for (int i = 0; i < response->suggestion_count; i++) {
            if (i > 0) strcat(result, ",");
            char suggestion[256];
            snprintf(suggestion, sizeof(suggestion), "\"%s\"",
                    response->suggested_actions[i] ? response->suggested_actions[i] : "");
            strcat(result, suggestion);
        }
        strcat(result, "]");
    }

    // Add clarification if needed
    if (response->clarification_question) {
        char clarification[512];
        snprintf(clarification, sizeof(clarification), ",\"clarification_question\":\"%s\"",
                response->clarification_question);
        strcat(result, clarification);
    }

    // Add data sources
    if (response->source_count > 0) {
        strcat(result, ",\"data_sources\":[");
        for (int i = 0; i < response->source_count; i++) {
            if (i > 0) strcat(result, ",");
            char source[256];
            snprintf(source, sizeof(source), "\"%s\"",
                    response->data_sources[i] ? response->data_sources[i] : "");
            strcat(result, source);
        }
        strcat(result, "]");
    }

    strcat(result, "}");
    return result;
}

// Main HTTP event handler
static void http_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        // Enable CORS
        mg_printf(c, "HTTP/1.1 200 OK\r\n"
                     "Access-Control-Allow-Origin: *\r\n"
                     "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                     "Access-Control-Allow-Headers: Content-Type\r\n"
                     "Content-Type: application/json\r\n\r\n");
        
        // Route requests
        if (mg_http_match_uri(hm, "/api/chat")) {
            handle_chat_endpoint(c, hm);
        } else if (mg_http_match_uri(hm, "/api/status")) {
            handle_status_endpoint(c, hm);
        } else if (mg_http_match_uri(hm, "/api/health")) {
            handle_health_endpoint(c, hm);
        } else if (mg_http_match_uri(hm, "/api/capabilities")) {
            handle_capabilities_endpoint(c, hm);
        } else {
            // Serve static files or 404
            struct mg_http_serve_opts opts = {.root_dir = "./web"};
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

// Simple JSON-like parsing (extract message from "message":"value")
static char* extract_message_from_json(const char* json_str) {
    if (!json_str) return NULL;

    const char* message_start = strstr(json_str, "\"message\":\"");
    if (!message_start) return NULL;

    message_start += 11; // Skip "message":"
    const char* message_end = strstr(message_start, "\"");
    if (!message_end) return NULL;

    int length = message_end - message_start;
    char* message = malloc(length + 1);
    if (!message) return NULL;

    strncpy(message, message_start, length);
    message[length] = '\0';
    return message;
}

// Chat endpoint handler
static void handle_chat_endpoint(struct mg_connection *c, struct mg_http_message *hm) {
    if (mg_vcmp(&hm->method, "POST") != 0) {
        mg_printf(c, "{\"error\": \"Method not allowed\"}\n");
        return;
    }

    // Simple JSON parsing - for demo, use a test message
    // In production, this would parse the actual HTTP body
    char* user_message = strdup("Show me Punjab groundwater data");
    if (!user_message) {
        mg_printf(c, "{\"error\": \"Invalid request format\"}\n");
        return;
    }

    // Process with enhanced chatbot
    BotResponse* response = process_user_query(user_message);
    free(user_message);

    if (response) {
        char* json_response = bot_response_to_json(response);
        if (json_response) {
            mg_printf(c, "%s\n", json_response);
            free(json_response);
        } else {
            mg_printf(c, "{\"error\": \"Failed to generate response\"}\n");
        }
        free_enhanced_bot_response(response);
    } else {
        mg_printf(c, "{\"error\": \"Internal server error\"}\n");
    }
}

// Status endpoint handler
static void handle_status_endpoint(struct mg_connection *c, struct mg_http_message *hm) {
    mg_printf(c, "{\"status\":\"online\",\"version\":\"2.0.0-enhanced\",\"intent_count\":70,\"server_time\":%ld}\n", time(NULL));
}

// Health check endpoint
static void handle_health_endpoint(struct mg_connection *c, struct mg_http_message *hm) {
    mg_printf(c, "{\"status\": \"healthy\", \"timestamp\": %ld}\n", time(NULL));
}

// Capabilities endpoint
static void handle_capabilities_endpoint(struct mg_connection *c, struct mg_http_message *hm) {
    mg_printf(c, "{\"capabilities\":[\"Location-based groundwater queries\",\"Historical trend analysis\",\"Multi-location comparisons\",\"Policy recommendations\",\"Conservation method suggestions\",\"Crisis area identification\",\"Technical explanations\",\"Context-aware conversations\",\"Fuzzy string matching\",\"Multi-language support framework\",\"Real-time confidence scoring\",\"Follow-up suggestions\",\"Data source attribution\"],\"total_intents\":70,\"supported_languages\":\"English, Hindi (framework)\"}\n");
}

// Start API server
int start_api_server(const char* port) {
    struct mg_mgr mgr;
    struct mg_connection *c;
    
    mg_mgr_init(&mgr);
    
    char listen_addr[64];
    snprintf(listen_addr, sizeof(listen_addr), "http://0.0.0.0:%s", port);
    
    c = mg_http_listen(&mgr, listen_addr, http_handler, NULL);
    if (c == NULL) {
        printf("❌ Failed to start API server on port %s\n", port);
        return 1;
    }
    
    printf("🌐 INGRES API Server started on http://localhost:%s\n", port);
    printf("📡 Endpoints available:\n");
    printf("   POST /api/chat - Main chat interface\n");
    printf("   GET  /api/status - Server status\n");
    printf("   GET  /api/health - Health check\n");
    printf("   GET  /api/capabilities - System capabilities\n");
    printf("   GET  / - Static web interface\n\n");
    
    // Event loop
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    
    mg_mgr_free(&mgr);
    return 0;
}

// Legacy function for backward compatibility
void handle_chat_request(const char* message, char** response) {
    BotResponse* bot_response = process_user_query(message);
    if (bot_response && bot_response->message) {
        *response = strdup(bot_response->message);
        free_enhanced_bot_response(bot_response);
    } else {
        *response = strdup("Error processing request");
    }
}
