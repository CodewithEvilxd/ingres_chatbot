#include "chatbot.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

// Test framework structures
typedef struct {
    const char* test_name;
    const char* input;
    IntentType expected_intent;
    float min_confidence;
    int should_pass;
} IntentTestCase;

typedef struct {
    const char* test_name;
    const char* input;
    int should_contain_response;
    const char* expected_substring;
} ResponseTestCase;

typedef struct {
    const char* test_name;
    const char* input;
    int should_pass;
} FuzzyMatchingTestCase;

typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    double total_time;
} TestResults;

// External logging function
extern void log_message(int level, const char* format, ...);

// Test data
IntentTestCase intent_tests[] = {
    // Basic interactions
    {"Greeting Test", "Hello", INTENT_GREETING, 0.8, 1},
    {"Help Test", "Help me", INTENT_HELP, 0.8, 1},
    {"Goodbye Test", "Bye", INTENT_GOODBYE, 0.8, 1},

    // Location queries
    {"Location Query Test", "Show me Punjab data", INTENT_QUERY_LOCATION, 0.7, 1},
    {"District Query Test", "Tell me about Amritsar", INTENT_QUERY_DISTRICT, 0.7, 1},

    // Critical areas
    {"Critical Areas Test", "Which areas are critical?", INTENT_CRITICAL_AREAS, 0.8, 1},
    {"Over-exploited Test", "Show over-exploited regions", INTENT_OVER_EXPLOITED_AREAS, 0.8, 1},

    // Comparisons
    {"Comparison Test", "Compare Punjab and Haryana", INTENT_COMPARE_LOCATIONS, 0.7, 1},

    // Technical queries
    {"Technical Test", "Explain stage of extraction", INTENT_TECHNICAL_EXPLANATION, 0.7, 1},

    // Policy queries
    {"Policy Test", "What are the policy recommendations?", INTENT_POLICY_SUGGESTION, 0.7, 1},

    // Fuzzy matching tests
    {"Fuzzy Punjab", "Show me Panjab data", INTENT_QUERY_LOCATION, 0.6, 1},
    {"Fuzzy Maharashtra", "Groundwater in Maharashtr", INTENT_QUERY_LOCATION, 0.6, 1},
};

ResponseTestCase response_tests[] = {
    {"Greeting Response", "Hello", 1, "Welcome to INGRES"},
    {"Help Response", "Help", 1, "I can help you with"},
    {"Critical Areas Response", "Show critical areas", 1, "CRITICAL GROUNDWATER AREAS"},
    {"Policy Response", "Policy suggestions", 1, "COMPREHENSIVE POLICY RECOMMENDATIONS"},
};

FuzzyMatchingTestCase fuzzy_tests[] = {
    {"Punjab Variations", "Show me Panjab groundwater data", 1},
    {"Maharashtra Variations", "Groundwater status in Maharashtr", 1},
    {"Haryana Variations", "Data for Hariyana", 1},
    {"Gujarat Variations", "Gujrat water crisis", 1},
    {"Rajasthan Variations", "Rajsthan groundwater", 1},
};

// Test functions
int run_intent_tests(TestResults* results) {
    int test_count = sizeof(intent_tests) / sizeof(IntentTestCase);
    int passed = 0;

    printf("\n🧪 INTENT CLASSIFICATION TESTS\n");
    printf("================================\n");

    for (int i = 0; i < test_count; i++) {
        clock_t start = clock();

        float confidence;
        IntentType result = classify_intent_advanced(intent_tests[i].input, NULL, &confidence);

        clock_t end = clock();
        double time_taken = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;

        int test_passed = (result == intent_tests[i].expected_intent &&
                          confidence >= intent_tests[i].min_confidence &&
                          intent_tests[i].should_pass);

        if (test_passed) {
            passed++;
            printf("✅ %s: PASSED (%.2fms)\n", intent_tests[i].test_name, time_taken);
            log_message(1, "Test '%s' PASSED", intent_tests[i].test_name);
        } else {
            printf("❌ %s: FAILED\n", intent_tests[i].test_name);
            printf("   Expected: %d (%.2f confidence)\n", intent_tests[i].expected_intent, intent_tests[i].min_confidence);
            printf("   Got: %d (%.2f confidence)\n", result, confidence);
            log_message(2, "Test '%s' FAILED - Expected %d, Got %d", intent_tests[i].test_name,
                       intent_tests[i].expected_intent, result);
        }

        results->total_time += time_taken;
    }

    results->total_tests += test_count;
    results->passed_tests += passed;
    results->failed_tests += (test_count - passed);

    printf("\nIntent Tests: %d/%d passed\n", passed, test_count);
    return passed;
}

int run_response_tests(TestResults* results) {
    int test_count = sizeof(response_tests) / sizeof(ResponseTestCase);
    int passed = 0;

    printf("\n📝 RESPONSE GENERATION TESTS\n");
    printf("=============================\n");

    for (int i = 0; i < test_count; i++) {
        clock_t start = clock();

        BotResponse* response = process_user_query(response_tests[i].input);

        clock_t end = clock();
        double time_taken = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;

        int test_passed = 0;
        if (response && response->message) {
            if (response_tests[i].should_contain_response) {
                test_passed = strstr(response->message, response_tests[i].expected_substring) != NULL;
            } else {
                test_passed = strstr(response->message, response_tests[i].expected_substring) == NULL;
            }
        }

        if (test_passed) {
            passed++;
            printf("✅ %s: PASSED (%.2fms)\n", response_tests[i].test_name, time_taken);
            log_message(1, "Response test '%s' PASSED", response_tests[i].test_name);
        } else {
            printf("❌ %s: FAILED\n", response_tests[i].test_name);
            printf("   Expected substring: '%s'\n", response_tests[i].expected_substring);
            if (response && response->message) {
                printf("   Response: '%.100s...'\n", response->message);
            } else {
                printf("   No response generated\n");
            }
            log_message(2, "Response test '%s' FAILED", response_tests[i].test_name);
        }

        if (response) {
            free_enhanced_bot_response(response);
        }

        results->total_time += time_taken;
    }

    results->total_tests += test_count;
    results->passed_tests += passed;
    results->failed_tests += (test_count - passed);

    printf("\nResponse Tests: %d/%d passed\n", passed, test_count);
    return passed;
}

int run_fuzzy_tests(TestResults* results) {
    int test_count = sizeof(fuzzy_tests) / sizeof(FuzzyMatchingTestCase);
    int passed = 0;

    printf("\n🔍 FUZZY MATCHING TESTS\n");
    printf("========================\n");

    for (int i = 0; i < test_count; i++) {
        clock_t start = clock();

        float confidence;
        IntentType result = classify_intent_advanced(fuzzy_tests[i].input, NULL, &confidence);

        clock_t end = clock();
        double time_taken = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;

        // For fuzzy tests, we just check if we get a reasonable intent classification
        int test_passed = (result != INTENT_UNKNOWN && result != INTENT_ERROR &&
                          confidence > 0.3 && fuzzy_tests[i].should_pass);

        if (test_passed) {
            passed++;
            printf("✅ %s: PASSED (%.2fms, confidence: %.2f)\n",
                   fuzzy_tests[i].test_name, time_taken, confidence);
            log_message(1, "Fuzzy test '%s' PASSED", fuzzy_tests[i].test_name);
        } else {
            printf("❌ %s: FAILED\n", fuzzy_tests[i].test_name);
            printf("   Intent: %d, Confidence: %.2f\n", result, confidence);
            log_message(2, "Fuzzy test '%s' FAILED - Intent: %d, Confidence: %.2f",
                       fuzzy_tests[i].test_name, result, confidence);
        }

        results->total_time += time_taken;
    }

    results->total_tests += test_count;
    results->passed_tests += passed;
    results->failed_tests += (test_count - passed);

    printf("\nFuzzy Tests: %d/%d passed\n", passed, test_count);
    return passed;
}

int run_performance_tests(TestResults* results) {
    printf("\n⚡ PERFORMANCE TESTS\n");
    printf("====================\n");

    const char* performance_queries[] = {
        "Show me Punjab data",
        "Compare Punjab and Haryana",
        "What are critical areas?",
        "Policy recommendations for Gujarat",
        "Explain groundwater categories",
        "Show rainfall impact",
        "Tell me about conservation methods"
    };

    int query_count = sizeof(performance_queries) / sizeof(char*);
    double total_time = 0.0;
    int successful_queries = 0;

    printf("Running %d performance queries...\n", query_count);

    for (int i = 0; i < query_count; i++) {
        clock_t start = clock();

        BotResponse* response = process_user_query(performance_queries[i]);

        clock_t end = clock();
        double query_time = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;

        if (response && response->message && strlen(response->message) > 0) {
            successful_queries++;
            total_time += query_time;
            printf("✅ Query %d: %.2fms\n", i+1, query_time);
        } else {
            printf("❌ Query %d: FAILED\n", i+1);
        }

        if (response) {
            free_enhanced_bot_response(response);
        }
    }

    double avg_time = successful_queries > 0 ? total_time / successful_queries : 0.0;
    double success_rate = (double)successful_queries / query_count * 100.0;

    printf("\nPerformance Results:\n");
    printf("• Average Response Time: %.2fms\n", avg_time);
    printf("• Success Rate: %.1f%%\n", success_rate);
    printf("• Total Queries: %d\n", query_count);

    // Performance criteria
    int perf_passed = 1;
    if (avg_time > 200.0) {
        printf("⚠️  WARNING: Average response time > 200ms\n");
        perf_passed = 0;
    }
    if (success_rate < 90.0) {
        printf("⚠️  WARNING: Success rate < 90%%\n");
        perf_passed = 0;
    }

    if (perf_passed) {
        printf("✅ Performance test: PASSED\n");
        results->passed_tests++;
        log_message(1, "Performance test PASSED - Avg: %.2fms, Success: %.1f%%", avg_time, success_rate);
    } else {
        printf("❌ Performance test: FAILED\n");
        results->failed_tests++;
        log_message(2, "Performance test FAILED - Avg: %.2fms, Success: %.1f%%", avg_time, success_rate);
    }

    results->total_tests++;
    results->total_time += total_time;

    return perf_passed;
}

void print_test_summary(TestResults* results) {
    printf("\n" "═══════════════════════════════════════════════════════════════\n");
    printf("📊 COMPREHENSIVE TEST SUITE RESULTS\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("📈 Total Tests: %d\n", results->total_tests);
    printf("✅ Passed: %d\n", results->passed_tests);
    printf("❌ Failed: %d\n", results->failed_tests);
    printf("📊 Success Rate: %.1f%%\n", (double)results->passed_tests / results->total_tests * 100.0);
    printf("⏱️  Total Time: %.2fms\n", results->total_time);
    printf("⚡ Average Time per Test: %.2fms\n", results->total_time / results->total_tests);

    // Assessment
    printf("\n🎯 TEST SUITE ASSESSMENT:\n");
    double success_rate = (double)results->passed_tests / results->total_tests * 100.0;

    if (success_rate >= 95.0) {
        printf("   🏆 EXCELLENT: >95%% success rate\n");
        printf("   🎉 System is highly reliable and ready for production\n");
    } else if (success_rate >= 90.0) {
        printf("   ✅ GOOD: >90%% success rate\n");
        printf("   📈 System performs well with minor issues to address\n");
    } else if (success_rate >= 80.0) {
        printf("   ⚠️  FAIR: >80%% success rate\n");
        printf("   🔧 System needs improvements before production\n");
    } else {
        printf("   ❌ POOR: <80%% success rate\n");
        printf("   🚨 System requires significant fixes\n");
    }

    printf("═══════════════════════════════════════════════════════════════\n");

    log_message(1, "Test suite completed - %d/%d tests passed (%.1f%%)",
                results->passed_tests, results->total_tests, success_rate);
}

int run_comprehensive_test_suite() {
    printf("🧪 INGRES ChatBot - Comprehensive Test Suite\n");
    printf("===========================================\n");
    printf("Smart India Hackathon 2025 - Quality Assurance\n\n");

    // Initialize test results
    TestResults results = {0, 0, 0, 0.0};

    // Initialize chatbot for testing
    if (!chatbot_init()) {
        printf("❌ Failed to initialize chatbot for testing\n");
        return 1;
    }

    // Run all test categories
    run_intent_tests(&results);
    run_response_tests(&results);
    run_fuzzy_tests(&results);
    run_performance_tests(&results);

    // Print final summary
    print_test_summary(&results);

    // Cleanup
    chatbot_cleanup();

    return results.failed_tests == 0 ? 0 : 1;
}

// Main test runner
int main() {
    return run_comprehensive_test_suite();
}