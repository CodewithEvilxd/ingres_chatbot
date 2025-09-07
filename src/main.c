#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "chatbot.h"
#include "utils.h"

// Enhanced logging system
typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG
} LogLevel;

// Performance monitoring structure
typedef struct {
    time_t start_time;
    int total_queries;
    double avg_response_time;
    size_t memory_usage;
    double cache_hit_rate;
    int error_count;
    int peak_concurrent_users;
} PerformanceMetrics;

void log_message(LogLevel level, const char* format, ...) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    const char* level_str;
    switch (level) {
        case LOG_INFO: level_str = "INFO"; break;
        case LOG_WARNING: level_str = "WARN"; break;
        case LOG_ERROR: level_str = "ERROR"; break;
        case LOG_DEBUG: level_str = "DEBUG"; break;
        default: level_str = "UNKNOWN"; break;
    }

    printf("[%s] [%s] ", timestamp, level_str);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");

    // Also write to log file
    FILE* log_file = fopen("ingres_chatbot.log", "a");
    if (log_file) {
        fprintf(log_file, "[%s] [%s] ", timestamp, level_str);
        va_list args2;
        va_start(args2, format);
        vfprintf(log_file, format, args2);
        va_end(args2);
        fprintf(log_file, "\n");
        fclose(log_file);
    }
}

void print_performance_report(PerformanceMetrics* metrics) {
    time_t uptime = time(NULL) - metrics->start_time;

    printf("\n" "═══════════════════════════════════════════════════════════════\n");
    printf("📊 PERFORMANCE REPORT - INGRES ChatBot\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("⏱️  Uptime: %ld seconds\n", uptime);
    printf("📈 Total Queries Processed: %d\n", metrics->total_queries);
    printf("⚡ Average Response Time: %.2f ms\n", metrics->avg_response_time);
    printf("🎯 Queries per Second: %.2f\n", (double)metrics->total_queries / uptime);
    printf("❌ Error Rate: %.2f%%\n", metrics->total_queries > 0 ?
           (double)metrics->error_count / metrics->total_queries * 100.0 : 0.0);
    printf("💾 Memory Usage: ~%zu MB\n", metrics->memory_usage / (1024 * 1024));
    printf("🎯 Cache Hit Rate: %.1f%%\n", metrics->cache_hit_rate * 100.0);
    printf("👥 Peak Concurrent Users: %d\n", metrics->peak_concurrent_users);

    // Performance assessment
    printf("\n🎖️  PERFORMANCE ASSESSMENT:\n");
    if (metrics->avg_response_time < 100.0) {
        printf("   ✅ Response Time: EXCELLENT (<100ms)\n");
    } else if (metrics->avg_response_time < 200.0) {
        printf("   ✅ Response Time: GOOD (<200ms)\n");
    } else {
        printf("   ⚠️  Response Time: NEEDS OPTIMIZATION\n");
    }

    double error_rate = metrics->total_queries > 0 ?
                       (double)metrics->error_count / metrics->total_queries * 100.0 : 0.0;
    if (error_rate < 1.0) {
        printf("   ✅ Error Rate: EXCELLENT (<1%%)\n");
    } else if (error_rate < 5.0) {
        printf("   ✅ Error Rate: GOOD (<5%%)\n");
    } else {
        printf("   ⚠️  Error Rate: NEEDS IMPROVEMENT\n");
    }

    printf("═══════════════════════════════════════════════════════════════\n\n");

    log_message(LOG_INFO, "Performance report generated - Avg response: %.2fms, Error rate: %.2f%%",
                metrics->avg_response_time, error_rate);
}

// Comment out API includes for now
// #include "api.h"

// Enhanced test queries showcasing new capabilities
const char* enhanced_test_queries[] = {
    // Basic interaction
    "Hello INGRES",
    "Help me understand groundwater",
    
    // Location queries with fuzzy matching
    "Show me Panjab data",  // Misspelled Punjab
    "Groundwater in Maharashtr",  // Misspelled Maharashtra
    "Tell me about Amritsar district",
    
    // Advanced analysis
    "Compare Punjab vs Haryana groundwater trends",
    "Historical trend for Gujarat over 5 years",
    "Which areas need emergency intervention?",
    
    // Technical queries
    "Explain stage of extraction methodology",
    "What conservation methods work best?",
    "Policy recommendations for over-exploited areas",
    
    // Environmental factors
    "How does monsoon affect groundwater recharge?",
    "Climate change impact on water resources",
    
    // Context-aware follow-ups (to be tested in sequence)
    "Tell me more about that",
    "What are the economic impacts?",
    "Show me success stories",
    
    // Complex queries
    "Show critical areas in northern India with policy recommendations",
    "Agriculture impact on groundwater in Punjab",
    
    "Goodbye"
};

void print_response_details(BotResponse* response) {
    if (!response) return;

    printf("🤖 **INGRES RESPONSE**\n");
    printf("Intent: %d | Confidence: %.2f | Time: %.2fms\n",
           response->intent, response->confidence_score, response->processing_time_ms);

    if (response->requires_clarification && response->clarification_question) {
        printf("⚠️  CLARIFICATION NEEDED: %s\n", response->clarification_question);
    }

    printf("\n%s\n", response->message ? response->message : "No response generated");

    if (response->suggestion_count > 0) {
        printf("\n💡 **SUGGESTED FOLLOW-UPS**:\n");
        for (int i = 0; i < response->suggestion_count; i++) {
            if (response->suggested_actions[i]) {
                printf("   %d. %s\n", i+1, response->suggested_actions[i]);
            }
        }
    }

    if (response->source_count > 0) {
        printf("\n📚 **DATA SOURCES**: ");
        for (int i = 0; i < response->source_count; i++) {
            if (response->data_sources[i]) {
                printf("%s", response->data_sources[i]);
                if (i < response->source_count - 1) printf(", ");
            }
        }
        printf("\n");
    }
}

int main() {
    log_message(LOG_INFO, "🌊 *** INGRES ChatBot - Enhanced AI System Starting *** 🌊");
    log_message(LOG_INFO, "India's Groundwater Resource Expert System");
    log_message(LOG_INFO, "Smart India Hackathon 2025 | Enhanced Version");

    // Performance monitoring initialization
    PerformanceMetrics perf_metrics = {0};
    perf_metrics.start_time = time(NULL);
    perf_metrics.total_queries = 0;
    perf_metrics.avg_response_time = 0.0;
    perf_metrics.memory_usage = 0;
    perf_metrics.cache_hit_rate = 0.0;

    log_message(LOG_INFO, "Performance monitoring initialized");

    printf("🌊 *** INGRES ChatBot - Enhanced AI System *** 🌊\n");
    printf("================================================\n");
    printf("India's Groundwater Resource Expert System\n");
    printf("Smart India Hackathon 2025 | Enhanced Version\n\n");

    // Initialize enhanced chatbot with error handling
    log_message(LOG_INFO, "Initializing chatbot system...");
    if (!chatbot_init()) {
        log_message(LOG_ERROR, "Failed to initialize INGRES ChatBot!");
        printf("❌ Failed to initialize INGRES ChatBot!\n");
        return 1;
    }
    log_message(LOG_INFO, "Chatbot initialization successful");
    
    printf("\n🚀 **ENHANCED FEATURES LOADED**:\n");
    printf("   ✅ 70+ Intent Types with Fuzzy Matching\n");
    printf("   ✅ Context-Aware Conversations\n");
    printf("   ✅ Multi-language Support Framework\n");
    printf("   ✅ Advanced Location Extraction\n");
    printf("   ✅ Confidence Scoring & Clarifications\n");
    printf("   ✅ Follow-up Suggestions\n");
    printf("   ✅ Comprehensive Response Templates\n\n");
    
    // Test enhanced queries
    printf("*** TESTING ENHANCED CAPABILITIES ***\n");
    printf("====================================\n\n");
    
    int num_tests = sizeof(enhanced_test_queries) / sizeof(enhanced_test_queries[0]);
    
    for (int i = 0; i < num_tests; i++) {
        clock_t query_start = clock();
        printf("👤 USER: %s\n", enhanced_test_queries[i]);

        // Use enhanced processing
        BotResponse* response = process_user_query(enhanced_test_queries[i]);

        if (response) {
            print_response_details(response);
            free_enhanced_bot_response(response);

            // Update performance metrics
            perf_metrics.total_queries++;
            clock_t query_end = clock();
            double query_time = ((double)(query_end - query_start) / CLOCKS_PER_SEC) * 1000.0;
            perf_metrics.avg_response_time = (perf_metrics.avg_response_time * (perf_metrics.total_queries - 1) + query_time) / perf_metrics.total_queries;

            log_message(LOG_DEBUG, "Query %d processed in %.2fms", i+1, query_time);
        } else {
            printf("🤖 BOT: Error processing request\n");
            perf_metrics.error_count++;
            log_message(LOG_WARNING, "Query %d failed to process", i+1);
        }

        printf("\n" "═══════════════════════════════════════════════════════════════\n\n");

        // Add small delay for readability
        #ifdef _WIN32
            system("timeout /t 1 >nul");
        #else
            system("sleep 1");
        #endif
    }

    // Print performance summary
    print_performance_report(&perf_metrics);
    
    // Interactive enhanced mode
    printf("*** ENHANCED INTERACTIVE MODE ***\n");
    printf("================================\n");
    printf("🎯 Features: Context awareness, fuzzy matching, follow-up suggestions\n");
    printf("💬 Try: 'Show Punjab crisis', then 'tell me more', then 'what solutions?'\n");
    printf("🔤 Test: Misspellings like 'Panjab' or 'Maharashtr'\n");
    printf("❓ Type 'quit' to exit\n\n");

    char input[MAX_INPUT_LENGTH];
    int query_number = 1;

    while (1) {
        printf("👤 [Q%d]: ", query_number++);
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0 || strcmp(input, "bye") == 0) {
            printf("🤖 Thank you for using INGRES ChatBot! Goodbye!\n");
            break;
        }

        if (strlen(input) == 0) {
            printf("🤖 Please enter a query or 'quit' to exit.\n\n");
            continue;
        }

        // Process with enhanced system
        BotResponse* response = process_user_query(input);

        if (response) {
            print_response_details(response);
            free_enhanced_bot_response(response);
        } else {
            printf("🤖 Sorry, I encountered an error processing your request.\n");
        }

        printf("\n");
    }
    
    // Cleanup
    chatbot_cleanup();
    printf("\n🎉 Thank you for testing INGRES ChatBot Enhanced System!\n");
    printf("💡 Ready for Smart India Hackathon 2025!\n");
    
    return 0;
}