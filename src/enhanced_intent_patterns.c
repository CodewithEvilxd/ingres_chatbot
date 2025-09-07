#include "chatbot.h"
#include "intent_patterns.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>


// Enhanced pattern structure with fuzzy matching and context
typedef struct {
    char* keywords[15];         // Increased keyword capacity
    char* synonyms[20];         // Synonym support for better matching
    char* context_keywords[10]; // Context-dependent keywords
    int keyword_count;
    int synonym_count;
    int context_count;
    int priority;
    IntentType intent;
    bool require_all;
    bool context_dependent;     // Whether this pattern needs context
    float min_confidence;       // Minimum confidence threshold
    char* example_queries[5];   // Example queries for this intent
    int example_count;
} EnhancedIntentPattern;

// Forward declarations for helper functions
bool is_stop_word(const char* word);
float calculate_advanced_similarity(const char* str1, const char* str2);
float calculate_jaccard_similarity(const char* str1, const char* str2);
float calculate_ngram_score(const char* input, EnhancedIntentPattern* pattern, int word_count, char* words[]);
float calculate_context_score(ConversationContext* context, EnhancedIntentPattern* pattern, const char* input);
bool is_related_intent(IntentType intent1, IntentType intent2);
float calculate_coverage_ratio(const char* input, EnhancedIntentPattern* pattern);

// Comprehensive enhanced patterns
EnhancedIntentPattern enhanced_patterns[] = {
    // === GREETING PATTERNS ===
    {
        .keywords = {"hello", "hi", "namaste", "good morning", "good evening", "hey", "greetings"},
        .synonyms = {"hola", "bonjour", "salaam", "vanakkam", "sat sri akal", "adaab"},
        .keyword_count = 7, .synonym_count = 6, .priority = 15, .intent = INTENT_GREETING,
        .require_all = false, .context_dependent = false, .min_confidence = 0.8,
        .example_queries = {"Hello", "Hi there", "Good morning", "Namaste", "Hey chatbot"},
        .example_count = 5
    },
    
    // === LOCATION QUERY PATTERNS ===
    {
        .keywords = {"show", "data", "for", "groundwater", "in"},
        .synonyms = {"display", "information", "details", "stats", "statistics"},
        .context_keywords = {"punjab", "haryana", "gujarat", "maharashtra", "rajasthan"},
        .keyword_count = 5, .synonym_count = 5, .context_count = 5, .priority = 12,
        .intent = INTENT_QUERY_LOCATION, .require_all = false, .context_dependent = true,
        .min_confidence = 0.7,
        .example_queries = {"Show me Punjab data", "Groundwater in Maharashtra", "Data for Gujarat"},
        .example_count = 3
    },
    
    {
        .keywords = {"district", "data", "show", "me"},
        .synonyms = {"information", "details", "stats", "display"},
        .context_keywords = {"amritsar", "ludhiana", "pune", "ahmedabad", "jaipur"},
        .keyword_count = 4, .synonym_count = 4, .context_count = 5, .priority = 13,
        .intent = INTENT_QUERY_DISTRICT, .require_all = false, .context_dependent = true,
        .min_confidence = 0.75,
        .example_queries = {"Show me Amritsar district", "Pune district data", "Ludhiana information"},
        .example_count = 3
    },
    
    // === CRITICAL AREAS PATTERNS ===
    {
        .keywords = {"critical", "areas", "which", "show"},
        .synonyms = {"dangerous", "problematic", "concerning", "alarming", "severe"},
        .context_keywords = {"over-exploited", "crisis", "emergency", "urgent"},
        .keyword_count = 4, .synonym_count = 5, .context_count = 4, .priority = 14,
        .intent = INTENT_CRITICAL_AREAS, .require_all = false, .context_dependent = false,
        .min_confidence = 0.8,
        .example_queries = {"Which areas are critical?", "Show critical regions", "Dangerous groundwater areas"},
        .example_count = 3
    },
    
    {
        .keywords = {"over-exploited", "over", "exploited", "areas"},
        .synonyms = {"overused", "depleted", "exhausted", "mining"},
        .keyword_count = 4, .synonym_count = 4, .priority = 15,
        .intent = INTENT_OVER_EXPLOITED_AREAS, .require_all = false, .context_dependent = false,
        .min_confidence = 0.85,
        .example_queries = {"Over-exploited areas", "Show overused regions", "Depleted groundwater zones"},
        .example_count = 3
    },
    
    // === COMPARISON PATTERNS ===
    {
        .keywords = {"compare", "vs", "versus", "difference", "between"},
        .synonyms = {"contrast", "differentiate", "analyze", "examine"},
        .context_keywords = {"punjab", "haryana", "gujarat", "states", "regions"},
        .keyword_count = 5, .synonym_count = 4, .context_count = 5, .priority = 13,
        .intent = INTENT_COMPARE_LOCATIONS, .require_all = false, .context_dependent = true,
        .min_confidence = 0.75,
        .example_queries = {"Compare Punjab vs Haryana", "Difference between Gujarat and Rajasthan"},
        .example_count = 2
    },
    
    // === TREND ANALYSIS PATTERNS ===
    {
        .keywords = {"trend", "historical", "over", "time", "change"},
        .synonyms = {"pattern", "evolution", "development", "progression", "trajectory"},
        .context_keywords = {"years", "decade", "annual", "monthly", "seasonal"},
        .keyword_count = 5, .synonym_count = 5, .context_count = 5, .priority = 12,
        .intent = INTENT_HISTORICAL_TREND, .require_all = false, .context_dependent = false,
        .min_confidence = 0.7,
        .example_queries = {"Historical trend for Punjab", "Groundwater change over time", "Trend analysis"},
        .example_count = 3
    },
    
    // === POLICY & MANAGEMENT PATTERNS ===
    {
        .keywords = {"policy", "suggestions", "recommendations", "what", "should"},
        .synonyms = {"advice", "guidance", "measures", "solutions", "strategies"},
        .context_keywords = {"government", "conservation", "management", "regulation"},
        .keyword_count = 5, .synonym_count = 5, .context_count = 4, .priority = 11,
        .intent = INTENT_POLICY_SUGGESTION, .require_all = false, .context_dependent = false,
        .min_confidence = 0.7,
        .example_queries = {"Policy suggestions for Punjab", "What should government do?", "Conservation measures"},
        .example_count = 3
    },
    
    {
        .keywords = {"conservation", "methods", "techniques", "how", "to", "save"},
        .synonyms = {"preservation", "protection", "sustainability", "efficiency"},
        .keyword_count = 6, .synonym_count = 4, .priority = 12,
        .intent = INTENT_CONSERVATION_METHODS, .require_all = false, .context_dependent = false,
        .min_confidence = 0.75,
        .example_queries = {"Water conservation methods", "How to save groundwater", "Conservation techniques"},
        .example_count = 3
    },
    
    // === ENVIRONMENTAL FACTORS ===
    {
        .keywords = {"rainfall", "monsoon", "affect", "impact", "groundwater"},
        .synonyms = {"precipitation", "rain", "weather", "climate", "influence"},
        .keyword_count = 5, .synonym_count = 5, .priority = 11,
        .intent = INTENT_RAINFALL_CORRELATION, .require_all = false, .context_dependent = false,
        .min_confidence = 0.7,
        .example_queries = {"How does rainfall affect groundwater?", "Monsoon impact on water table"},
        .example_count = 2
    },
    
    // === AGRICULTURAL PATTERNS ===
    {
        .keywords = {"agriculture", "farming", "crops", "irrigation", "impact"},
        .synonyms = {"cultivation", "agricultural", "farm", "crop", "harvest"},
        .context_keywords = {"rice", "wheat", "sugarcane", "cotton", "water-intensive"},
        .keyword_count = 5, .synonym_count = 5, .context_count = 5, .priority = 10,
        .intent = INTENT_AGRICULTURE_IMPACT, .require_all = false, .context_dependent = false,
        .min_confidence = 0.65,
        .example_queries = {"Agriculture impact on groundwater", "Farming effects on water table"},
        .example_count = 2
    },
    
    // === TECHNICAL EXPLANATION PATTERNS ===
    {
        .keywords = {"explain", "what", "is", "stage", "extraction", "mean"},
        .synonyms = {"define", "clarify", "describe", "elaborate", "meaning"},
        .context_keywords = {"technical", "calculation", "methodology", "formula"},
        .keyword_count = 6, .synonym_count = 5, .context_count = 4, .priority = 11,
        .intent = INTENT_TECHNICAL_EXPLANATION, .require_all = false, .context_dependent = false,
        .min_confidence = 0.7,
        .example_queries = {"What is stage of extraction?", "Explain groundwater categories"},
        .example_count = 2
    },
    
    // === CRISIS & EMERGENCY PATTERNS ===
    {
        .keywords = {"water", "crisis", "emergency", "shortage", "scarcity"},
        .synonyms = {"drought", "deficit", "lack", "depletion", "stress"},
        .context_keywords = {"urgent", "immediate", "critical", "severe"},
        .keyword_count = 5, .synonym_count = 5, .context_count = 4, .priority = 13,
        .intent = INTENT_WATER_CRISIS, .require_all = false, .context_dependent = false,
        .min_confidence = 0.75,
        .example_queries = {"Water crisis areas", "Emergency water shortage", "Drought affected regions"},
        .example_count = 3
    },
    
    // === ECONOMIC & SOCIAL IMPACT ===
    {
        .keywords = {"economic", "cost", "impact", "financial", "social"},
        .synonyms = {"monetary", "expense", "budget", "society", "community"},
        .keyword_count = 5, .synonym_count = 5, .priority = 9,
        .intent = INTENT_ECONOMIC_IMPACT, .require_all = false, .context_dependent = false,
        .min_confidence = 0.65,
        .example_queries = {"Economic impact of water crisis", "Cost of groundwater depletion"},
        .example_count = 2
    },
    
    // === FOLLOW-UP & CONTEXT PATTERNS ===
    {
        .keywords = {"tell", "me", "more", "about", "that", "elaborate"},
        .synonyms = {"explain", "details", "further", "additional", "expand"},
        .keyword_count = 6, .synonym_count = 5, .priority = 8,
        .intent = INTENT_FOLLOW_UP_QUESTION, .require_all = false, .context_dependent = true,
        .min_confidence = 0.6,
        .example_queries = {"Tell me more about that", "Can you elaborate?", "More details please"},
        .example_count = 3
    },
    
    // === HELP & GUIDANCE ===
    {
        .keywords = {"help", "how", "to", "use", "guide", "commands"},
        .synonyms = {"assistance", "support", "instructions", "tutorial"},
        .keyword_count = 6, .synonym_count = 4, .priority = 14,
        .intent = INTENT_HELP, .require_all = false, .context_dependent = false,
        .min_confidence = 0.8,
        .example_queries = {"Help me", "How to use this?", "What can you do?", "Commands list"},
        .example_count = 4
    }
};

int enhanced_pattern_count = sizeof(enhanced_patterns) / sizeof(EnhancedIntentPattern);

// Indian state and city names for location extraction
const char* indian_states[] = {
    "andhra pradesh", "arunachal pradesh", "assam", "bihar", "chhattisgarh", "goa", "gujarat",
    "haryana", "himachal pradesh", "jharkhand", "karnataka", "kerala", "madhya pradesh",
    "maharashtra", "manipur", "meghalaya", "mizoram", "nagaland", "odisha", "punjab",
    "rajasthan", "sikkim", "tamil nadu", "telangana", "tripura", "uttar pradesh",
    "uttarakhand", "west bengal", "delhi", "jammu and kashmir", "ladakh"
};

const char* major_cities[] = {
    "mumbai", "delhi", "bangalore", "hyderabad", "ahmedabad", "chennai", "kolkata",
    "surat", "pune", "jaipur", "lucknow", "kanpur", "nagpur", "indore", "thane",
    "bhopal", "visakhapatnam", "pimpri", "patna", "vadodara", "ghaziabad", "ludhiana",
    "agra", "nashik", "faridabad", "meerut", "rajkot", "kalyan", "vasai", "varanasi",
    "srinagar", "aurangabad", "dhanbad", "amritsar", "navi mumbai", "allahabad",
    "ranchi", "howrah", "coimbatore", "jabalpur", "gwalior", "vijayawada", "jodhpur",
    "madurai", "raipur", "kota", "chandigarh", "guwahati", "solapur", "hubli"
};

int state_count = sizeof(indian_states) / sizeof(char*);
int city_count = sizeof(major_cities) / sizeof(char*);

// Levenshtein distance for fuzzy matching
int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1), len2 = strlen(s2);
    int matrix[len1 + 1][len2 + 1];
    
    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;
    
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            matrix[i][j] = fmin(fmin(matrix[i-1][j] + 1, matrix[i][j-1] + 1), 
                               matrix[i-1][j-1] + cost);
        }
    }
    
    return matrix[len1][len2];
}

float calculate_similarity(const char* str1, const char* str2) {
    if (!str1 || !str2) return 0.0;
    
    int len1 = strlen(str1), len2 = strlen(str2);
    if (len1 == 0 && len2 == 0) return 1.0;
    if (len1 == 0 || len2 == 0) return 0.0;
    
    int distance = levenshtein_distance(str1, str2);
    int max_len = (len1 > len2) ? len1 : len2;
    
    return 1.0 - ((float)distance / max_len);
}

// Enhanced pattern matching with advanced fuzzy logic and N-gram analysis
IntentType classify_intent_advanced(const char* user_input, ConversationContext* context, float* confidence) {
    if (!user_input) {
        *confidence = 0.0;
        return INTENT_ERROR;
    }

    char* lower_input = string_to_lower((char*)user_input);
    if (!lower_input) {
        *confidence = 0.0;
        return INTENT_ERROR;
    }

    float best_score = 0.0;
    IntentType best_intent = INTENT_UNKNOWN;

    // Tokenize input for advanced analysis
    char* words[100];
    int word_count = 0;
    char* input_copy = strdup(lower_input);
    char* token = strtok(input_copy, " ,.!?;:\"'()");

    while (token && word_count < 100) {
        // Skip very short words and common stop words
        if (strlen(token) > 1 && !is_stop_word(token)) {
            words[word_count++] = token;
        }
        token = strtok(NULL, " ,.!?;:\"'()");
    }

    for (int i = 0; i < enhanced_pattern_count; i++) {
        EnhancedIntentPattern* pattern = &enhanced_patterns[i];
        float score = 0.0;
        int exact_matches = 0;
        int fuzzy_matches = 0;
        int synonym_matches = 0;

        // 1. Exact keyword matching (highest weight)
        for (int j = 0; j < pattern->keyword_count; j++) {
            if (strstr(lower_input, pattern->keywords[j])) {
                exact_matches++;
                score += 1.2;  // Higher weight for exact matches
            }
        }

        // 2. Enhanced synonym matching
        for (int j = 0; j < pattern->synonym_count; j++) {
            if (strstr(lower_input, pattern->synonyms[j])) {
                synonym_matches++;
                score += 0.9;  // Good weight for synonyms
            }
        }

        // 3. Advanced fuzzy matching with multiple algorithms
        for (int j = 0; j < pattern->keyword_count; j++) {
            for (int k = 0; k < word_count; k++) {
                float similarity = calculate_advanced_similarity(pattern->keywords[j], words[k]);
                if (similarity > 0.75) {  // Stricter threshold
                    fuzzy_matches++;
                    score += similarity * 0.7;  // Good weight for fuzzy matches
                }
            }
        }

        // 4. N-gram matching for better context understanding
        score += calculate_ngram_score(lower_input, pattern, word_count, words);

        // 5. Context-dependent scoring with memory
        if (pattern->context_dependent && context) {
            score += calculate_context_score(context, pattern, lower_input);
        }

        // 6. Pattern-specific scoring adjustments
        if (pattern->require_all) {
            if (exact_matches < pattern->keyword_count) {
                score *= 0.6;  // Penalty for missing required keywords
            }
        }

        // 7. Length-based scoring (prefer patterns that match more of the input)
        float coverage_ratio = calculate_coverage_ratio(lower_input, pattern);
        score *= (0.8 + 0.2 * coverage_ratio);

        // 8. Apply priority weighting with dynamic adjustment
        float priority_multiplier = pattern->priority / 10.0;
        if (exact_matches > 0) {
            priority_multiplier *= 1.2;  // Boost for exact matches
        }
        score *= priority_multiplier;

        // 9. Normalize and apply confidence threshold
        float normalized_score = score / (pattern->keyword_count + pattern->synonym_count + 1.0);

        // Boost score for patterns with high match quality
        if (exact_matches > 0 && fuzzy_matches > 0) {
            normalized_score *= 1.1;  // Bonus for mixed matching
        }

        if (normalized_score >= pattern->min_confidence && normalized_score > best_score) {
            best_score = normalized_score;
            best_intent = pattern->intent;
        }
    }

    free(input_copy);
    free(lower_input);

    *confidence = best_score;

    // Return UNKNOWN if confidence is too low
    return (best_score > 0.35) ? best_intent : INTENT_UNKNOWN;
}

// Helper function to check if word is a stop word
bool is_stop_word(const char* word) {
    const char* stop_words[] = {
        "the", "a", "an", "and", "or", "but", "in", "on", "at", "to", "for", "of", "with", "by",
        "is", "are", "was", "were", "be", "been", "being", "have", "has", "had", "do", "does",
        "did", "will", "would", "could", "should", "may", "might", "must", "can", "shall"
    };

    for (int i = 0; i < sizeof(stop_words) / sizeof(stop_words[0]); i++) {
        if (strcmp(word, stop_words[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Advanced similarity calculation with multiple algorithms
float calculate_advanced_similarity(const char* str1, const char* str2) {
    if (!str1 || !str2) return 0.0;

    // Use Levenshtein distance as base
    float levenshtein_sim = calculate_similarity(str1, str2);

    // Add Jaccard similarity for better results
    float jaccard_sim = calculate_jaccard_similarity(str1, str2);

    // Add length ratio factor
    float length_ratio = 1.0 - fabs(strlen(str1) - strlen(str2)) / (float)fmax(strlen(str1), strlen(str2));

    // Weighted combination
    return (levenshtein_sim * 0.6) + (jaccard_sim * 0.3) + (length_ratio * 0.1);
}

// Jaccard similarity for set-based comparison
float calculate_jaccard_similarity(const char* str1, const char* str2) {
    // Simple character-based Jaccard
    bool chars1[256] = {false};
    bool chars2[256] = {false};

    for (int i = 0; str1[i]; i++) {
        chars1[(unsigned char)str1[i]] = true;
    }
    for (int i = 0; str2[i]; i++) {
        chars2[(unsigned char)str2[i]] = true;
    }

    int intersection = 0;
    int union_count = 0;

    for (int i = 0; i < 256; i++) {
        if (chars1[i] || chars2[i]) union_count++;
        if (chars1[i] && chars2[i]) intersection++;
    }

    return union_count > 0 ? (float)intersection / union_count : 0.0;
}

// N-gram scoring for better context understanding
float calculate_ngram_score(const char* input, EnhancedIntentPattern* pattern, int word_count, char* words[]) {
    float score = 0.0;

    // Generate bigrams from input
    for (int i = 0; i < word_count - 1; i++) {
        char bigram[100];
        snprintf(bigram, sizeof(bigram), "%s %s", words[i], words[i+1]);

        // Check if bigram matches any pattern keywords
        for (int j = 0; j < pattern->keyword_count; j++) {
            if (strstr(bigram, pattern->keywords[j])) {
                score += 0.8;  // Bigram match bonus
            }
        }
    }

    return score;
}

// Context scoring with conversation memory
float calculate_context_score(ConversationContext* context, EnhancedIntentPattern* pattern, const char* input) {
    float score = 0.0;

    if (!context) return 0.0;

    // Check location context
    if (context->last_location) {
        for (int j = 0; j < pattern->context_count; j++) {
            if (strstr(context->last_location, pattern->context_keywords[j]) ||
                strstr(input, pattern->context_keywords[j])) {
                score += 0.6;  // Location context bonus
            }
        }
    }

    // Check intent history
    if (context->last_intent != INTENT_UNKNOWN) {
        // Boost related intents
        if (is_related_intent(context->last_intent, pattern->intent)) {
            score += 0.4;  // Related intent bonus
        }
    }

    return score;
}

// Check if two intents are related
bool is_related_intent(IntentType intent1, IntentType intent2) {
    // Define related intent groups
    if ((intent1 == INTENT_QUERY_LOCATION && intent2 == INTENT_COMPARE_LOCATIONS) ||
        (intent1 == INTENT_CRITICAL_AREAS && intent2 == INTENT_POLICY_SUGGESTION) ||
        (intent1 == INTENT_HISTORICAL_TREND && intent2 == INTENT_COMPARE_LOCATIONS) ||
        (intent1 == INTENT_WATER_CRISIS && intent2 == INTENT_CONSERVATION_METHODS)) {
        return true;
    }
    return false;
}

// Calculate how much of the input is covered by pattern matches
float calculate_coverage_ratio(const char* input, EnhancedIntentPattern* pattern) {
    int input_len = strlen(input);
    int covered_len = 0;

    // Count characters covered by matched keywords
    for (int j = 0; j < pattern->keyword_count; j++) {
        const char* pos = input;
        while ((pos = strstr(pos, pattern->keywords[j]))) {
            covered_len += strlen(pattern->keywords[j]);
            pos += strlen(pattern->keywords[j]);
        }
    }

    return input_len > 0 ? (float)covered_len / input_len : 0.0;
}

// Extract locations from user input
int extract_locations(const char* user_input, char** state, char** district, char** block) {
    if (!user_input) return 0;
    
    char* lower_input = string_to_lower((char*)user_input);
    int locations_found = 0;
    
    *state = NULL;
    *district = NULL;
    *block = NULL;
    
    // Check for state names
    for (int i = 0; i < state_count; i++) {
        if (strstr(lower_input, indian_states[i])) {
            *state = strdup(indian_states[i]);
            locations_found++;
            break;
        }
    }
    
    // Check for city names (could be districts)
    for (int i = 0; i < city_count; i++) {
        if (strstr(lower_input, major_cities[i])) {
            *district = strdup(major_cities[i]);
            locations_found++;
            break;
        }
    }
    
    // Fuzzy matching for misspelled locations
    if (locations_found == 0) {
        char* words[20];
        int word_count = 0;
        char* input_copy = strdup(lower_input);
        char* token = strtok(input_copy, " ");
        
        while (token && word_count < 20) {
            words[word_count++] = token;
            token = strtok(NULL, " ");
        }
        
        // Check each word against state names
        for (int i = 0; i < word_count; i++) {
            for (int j = 0; j < state_count; j++) {
                float similarity = calculate_similarity(words[i], indian_states[j]);
                if (similarity > 0.8) {  // 80% similarity for location names
                    *state = strdup(indian_states[j]);
                    locations_found++;
                    break;
                }
            }
            if (*state) break;
        }
        
        free(input_copy);
    }
    
    free(lower_input);
    return locations_found;
}

// Initialize conversation context
ConversationContext* init_conversation_context(void) {
    ConversationContext* context = malloc(sizeof(ConversationContext));
    if (!context) return NULL;
    
    context->last_location = NULL;
    context->last_state = NULL;
    context->last_district = NULL;
    context->last_intent = INTENT_UNKNOWN;
    context->history_count = 0;
    context->awaiting_clarification = false;
    context->pending_question = NULL;
    context->session_start = time(NULL);
    context->query_count = 0;
    
    for (int i = 0; i < 10; i++) {
        context->conversation_history[i] = NULL;
    }
    
    return context;
}

// Update conversation context
void update_conversation_context(ConversationContext* context, const char* user_input, 
                               IntentType intent, const char* location) {
    if (!context) return;
    
    // Update last intent
    context->last_intent = intent;
    context->query_count++;
    
    // Update location context
    if (location) {
        if (context->last_location) free(context->last_location);
        context->last_location = strdup(location);
    }
    
    // Add to conversation history
    if (context->history_count < 10) {
        context->conversation_history[context->history_count] = strdup(user_input);
        context->history_count++;
    } else {
        // Shift history and add new entry
        free(context->conversation_history[0]);
        for (int i = 0; i < 9; i++) {
            context->conversation_history[i] = context->conversation_history[i + 1];
        }
        context->conversation_history[9] = strdup(user_input);
    }
}

// Free conversation context
void free_conversation_context(ConversationContext* context) {
    if (!context) return;
    
    if (context->last_location) free(context->last_location);
    if (context->last_state) free(context->last_state);
    if (context->last_district) free(context->last_district);
    if (context->pending_question) free(context->pending_question);
    
    for (int i = 0; i < context->history_count; i++) {
        if (context->conversation_history[i]) {
            free(context->conversation_history[i]);
        }
    }
    
    free(context);
}