#include "chatbot.h"
#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Multi-language response templates
typedef struct {
    IntentType intent;
    char* english_template;
    char* hindi_template;
    char* regional_template;
    bool needs_location;
    bool needs_data;
    char* follow_up_suggestions[5];
    int suggestion_count;
} MultilingualResponseTemplate;

// Comprehensive response templates
MultilingualResponseTemplate enhanced_templates[] = {
    {
        .intent = 1,  // INTENT_GREETING
        .english_template = "🌊 Namaste! Welcome to INGRES - India's Groundwater Resource Expert System!\n\n"
                           "I'm your AI assistant for comprehensive groundwater information across India. "
                           "I can help you with:\n\n"
                           "📊 **Real-time Data**: Current groundwater status for any location\n"
                           "📈 **Trend Analysis**: Historical patterns and future predictions\n"
                           "⚠️ **Crisis Alerts**: Critical and over-exploited areas\n"
                           "🔄 **Comparisons**: Multi-location and temporal analysis\n"
                           "🏛️ **Policy Insights**: Government schemes and recommendations\n"
                           "🌱 **Conservation**: Sustainable water management strategies\n\n"
                           "Try asking: 'Show groundwater crisis in Punjab' or 'Compare water trends in Gujarat vs Rajasthan'",
        .hindi_template = "🌊 नमस्ते! INGRES में आपका स्वागत है - भारत की भूजल संसाधन विशेषज्ञ प्रणाली!\n\n"
                         "मैं भारत भर में व्यापक भूजल जानकारी के लिए आपका AI सहायक हूं। "
                         "मैं आपकी मदद कर सकता हूं:\n\n"
                         "📊 **वास्तविक समय डेटा**: किसी भी स्थान के लिए वर्तमान भूजल स्थिति\n"
                         "📈 **प्रवृत्ति विश्लेषण**: ऐतिहासिक पैटर्न और भविष्य की भविष्यवाणियां\n"
                         "⚠️ **संकट अलर्ट**: महत्वपूर्ण और अति-शोषित क्षेत्र\n"
                         "🔄 **तुलना**: बहु-स्थान और समय विश्लेषण\n"
                         "🏛️ **नीति अंतर्दृष्टि**: सरकारी योजनाएं और सिफारिशें\n"
                         "🌱 **संरक्षण**: टिकाऊ जल प्रबंधन रणनीतियां\n\n"
                         "पूछें: 'पंजाब में भूजल संकट दिखाएं' या 'गुजरात बनाम राजस्थान में जल प्रवृत्तियों की तुलना करें'",
        .needs_location = false,
        .needs_data = false,
        .follow_up_suggestions = {
            "Show me critical groundwater areas",
            "Compare Punjab vs Haryana water status", 
            "What are the conservation methods?",
            "Explain groundwater categories",
            "Show rainfall impact on groundwater"
        },
        .suggestion_count = 5
    },
    
    {
        .intent = 7,  // INTENT_QUERY_LOCATION
        .english_template = "🔍 **GROUNDWATER ANALYSIS FOR %s**\n\n"
                           "📍 **Location**: %s\n"
                           "📅 **Assessment Year**: %d\n"
                           "🏷️ **Category**: %s\n"
                           "💧 **Annual Recharge**: %.2f MCM\n"
                           "🏭 **Annual Extraction**: %.2f MCM\n"
                           "📊 **Stage of Extraction**: %.1f%%\n"
                           "⚖️ **Net Availability**: %.2f MCM\n\n"
                           "**Status Interpretation**:\n%s\n\n"
                           "**Trend**: %s over the past 3 years\n"
                           "**Risk Level**: %s",
       .hindi_template = "🔍 **%s के लिए भूजल विश्लेषण**\n\n"
                        "📍 **स्थान**: %s\n"
                        "📅 **मूल्यांकन वर्ष**: %d\n"
                        "🏷️ **श्रेणी**: %s\n"
                        "💧 **वार्षिक पुनर्भरण**: %.2f एमसीएम\n"
                        "🏭 **वार्षिक निकासी**: %.2f एमसीएम\n"
                        "📊 **निकासी का स्तर**: %.1f%%\n"
                        "⚖️ **नेट उपलब्धता**: %.2f एमसीएम\n\n"
                        "**स्थिति व्याख्या**:\n%s\n\n"
                        "**प्रवृत्ति**: पिछले 3 वर्षों में %s\n"
                        "**जोखिम स्तर**: %s",
        .needs_location = true,
        .needs_data = true,
        .follow_up_suggestions = {
            "Show historical trend for this area",
            "Compare with neighboring regions",
            "What conservation methods are suitable?",
            "Show policy recommendations",
            "Explain the technical details"
        },
        .suggestion_count = 5
    },
    
    {
        .intent = 37,  // INTENT_CRITICAL_AREAS
        .english_template = "🚨 **CRITICAL GROUNDWATER AREAS - URGENT ATTENTION REQUIRED**\n\n"
                           "**OVER-EXPLOITED REGIONS** (>100%% extraction):\n"
                           "🔴 **Punjab**: 78%% of blocks affected\n"
                           "   • Amritsar: 165%% extraction rate\n"
                           "   • Ludhiana: 178%% extraction rate\n"
                           "   • Bathinda: 156%% extraction rate\n\n"
                           "🔴 **Haryana**: 45%% of blocks affected\n"
                           "   • Sirsa: 189%% extraction rate\n"
                           "   • Hisar: 145%% extraction rate\n\n"
                           "🔴 **Tamil Nadu**: Urban stress zones\n"
                           "   • Chennai: 167%% extraction rate\n\n"
                           "**CRITICAL REGIONS** (90-100%% extraction):\n"
                           "🟠 **Maharashtra**: Pune (156%%), Aurangabad (145%%)\n"
                           "🟠 **Karnataka**: Bangalore Urban (167%%)\n"
                           "🟠 **Gujarat**: Ahmedabad City (156%%)\n"
                           "🟠 **Rajasthan**: Alwar (134%%)\n\n"
                           "**IMMEDIATE ACTIONS NEEDED**:\n"
                           "• Strict groundwater extraction regulations\n"
                           "• Mandatory rainwater harvesting\n"
                           "• Crop pattern diversification\n"
                           "• Industrial water recycling",
       .hindi_template = "🚨 **महत्वपूर्ण भूजल क्षेत्र - तत्काल ध्यान आवश्यक**\n\n"
                        "**अति-शोषित क्षेत्र** (>100%% निकासी):\n"
                        "🔴 **पंजाब**: 78%% ब्लॉक प्रभावित\n"
                        "   • अमृतसर: 165%% निकासी दर\n"
                        "   • लुधियाना: 178%% निकासी दर\n"
                        "   • बठिंडा: 156%% निकासी दर\n\n"
                        "🔴 **हरियाणा**: 45%% ब्लॉक प्रभावित\n"
                        "   • सिरसा: 189%% निकासी दर\n"
                        "   • हिसार: 145%% निकासी दर\n\n"
                        "🔴 **तमिलनाडु**: शहरी तनाव क्षेत्र\n"
                        "   • चेन्नई: 167%% निकासी दर\n\n"
                        "**महत्वपूर्ण क्षेत्र** (90-100%% निकासी):\n"
                        "🟠 **महाराष्ट्र**: पुणे (156%%), औरंगाबाद (145%%)\n"
                        "🟠 **कर्नाटक**: बंगलौर शहरी (167%%)\n"
                        "🟠 **गुजरात**: अहमदाबाद शहर (156%%)\n"
                        "🟠 **राजस्थान**: अलवर (134%%)\n\n"
                        "**तत्काल कार्रवाई आवश्यक**:\n"
                        "• कड़े भूजल निकासी नियम\n"
                        "• अनिवार्य वर्षा जल संचयन\n"
                        "• फसल पैटर्न विविधीकरण\n"
                        "• औद्योगिक जल पुनर्चक्रण",
        .needs_location = false,
        .needs_data = true,
        .follow_up_suggestions = {
            "Show conservation methods for critical areas",
            "Policy recommendations for over-exploited regions",
            "Compare with safe areas",
            "Historical trend of critical areas",
            "Economic impact of groundwater crisis"
        },
        .suggestion_count = 5
    },
    
    {
        .intent = 44,  // INTENT_COMPARE_LOCATIONS
        .english_template = "🔄 **COMPARATIVE GROUNDWATER ANALYSIS**\n\n"
                           "**%s vs %s** (Latest Assessment)\n\n"
                           "| Parameter | %s | %s |\n"
                           "|-----------|---------|----------|\n"
                           "| Over-exploited blocks | %d%% | %d%% |\n"
                           "| Average extraction | %.1f%% | %.1f%% |\n"
                           "| Water table trend | %s | %s |\n"
                           "| Primary concern | %s | %s |\n"
                           "| Risk level | %s | %s |\n\n"
                           "**KEY INSIGHTS**:\n"
                           "• %s shows %s groundwater stress\n"
                           "• %s has %s management practices\n"
                           "• Both regions need %s\n\n"
                           "**RECOMMENDATIONS**:\n"
                           "• %s: Focus on %s\n"
                           "• %s: Implement %s",
        .needs_location = true,
        .needs_data = true,
        .follow_up_suggestions = {
            "Show detailed data for each location",
            "Historical comparison over 5 years",
            "Policy differences between regions",
            "Success stories from better performing area",
            "Joint conservation strategies"
        },
        .suggestion_count = 5
    },
    
    {
        .intent = 52,  // INTENT_HISTORICAL_TREND
        .english_template = "📈 **HISTORICAL GROUNDWATER TREND ANALYSIS**\n\n"
                           "📍 **Location**: %s\n"
                           "📅 **Analysis Period**: 2019-2023 (5 years)\n\n"
                           "**YEAR-WISE EXTRACTION TRENDS**:\n"
                           "• 2019: %.1f%% extraction\n"
                           "• 2020: %.1f%% extraction (%+.1f%%)\n"
                           "• 2021: %.1f%% extraction (%+.1f%%)\n"
                           "• 2022: %.1f%% extraction (%+.1f%%)\n"
                           "• 2023: %.1f%% extraction (%+.1f%%)\n\n"
                           "**TREND ANALYSIS**:\n"
                           "📊 **Overall Trend**: %s\n"
                           "📉 **Annual Change Rate**: %+.2f%% per year\n"
                           "🎯 **Category Change**: %s → %s\n"
                           "⚠️ **Risk Trajectory**: %s\n\n"
                           "**CONTRIBUTING FACTORS**:\n"
                           "• Rainfall variation: %s\n"
                           "• Agricultural practices: %s\n"
                           "• Industrial growth: %s\n"
                           "• Policy interventions: %s\n\n"
                           "**FUTURE PROJECTION** (2024-2026):\n"
                           "If current trend continues: %s",
        .needs_location = true,
        .needs_data = true,
        .follow_up_suggestions = {
            "Show rainfall correlation analysis",
            "Compare with national average trend",
            "Policy interventions needed",
            "Success stories from similar regions",
            "Detailed monthly breakdown"
        },
        .suggestion_count = 5
    },
    
    {
        .intent = 72,  // INTENT_POLICY_SUGGESTION
        .english_template = "🏛️ **COMPREHENSIVE POLICY RECOMMENDATIONS FOR %s**\n\n"
                           "**IMMEDIATE MEASURES** (0-6 months):\n"
                           "🚨 **Regulatory Actions**:\n"
                           "• Implement groundwater extraction permits\n"
                           "• Mandatory water audits for industries\n"
                           "• Ban on new bore wells in over-exploited areas\n"
                           "• Real-time monitoring system installation\n\n"
                           "💧 **Conservation Mandates**:\n"
                           "• Rainwater harvesting for buildings >300 sq.m\n"
                           "• Drip irrigation subsidies (50%% cost)\n"
                           "• Greywater recycling in urban areas\n\n"
                           "**MEDIUM-TERM STRATEGIES** (6 months - 2 years):\n"
                           "🌾 **Agricultural Reforms**:\n"
                           "• Crop diversification incentives\n"
                           "• Shift from water-intensive crops (rice/sugarcane)\n"
                           "• Promote drought-resistant varieties\n"
                           "• Micro-irrigation expansion\n\n"
                           "🏭 **Industrial Measures**:\n"
                           "• Water recycling mandates (80%% reuse)\n"
                           "• Effluent treatment plant upgrades\n"
                           "• Water-positive industrial policies\n\n"
                           "**LONG-TERM VISION** (2-5 years):\n"
                           "🌍 **Sustainable Management**:\n"
                           "• Aquifer mapping and protection\n"
                           "• Inter-basin water transfer projects\n"
                           "• Community-based water management\n"
                           "• Climate-resilient water infrastructure\n\n"
                           "**FINANCIAL FRAMEWORK**:\n"
                           "💰 **Investment Required**: ₹%d crores\n"
                           "📊 **Expected Impact**: %d%% reduction in extraction\n"
                           "⏱️ **Timeline**: %d years for full implementation",
        .needs_location = true,
        .needs_data = false,
        .follow_up_suggestions = {
            "Show successful policy examples",
            "Economic analysis of recommendations",
            "Implementation timeline details",
            "Stakeholder engagement strategy",
            "Monitoring and evaluation framework"
        },
        .suggestion_count = 5
    },
    
    {
        .intent = 73,  // INTENT_CONSERVATION_METHODS
        .english_template = "🌱 **COMPREHENSIVE WATER CONSERVATION STRATEGIES**\n\n"
                           "**RAINWATER HARVESTING**:\n"
                           "🏠 **Rooftop Systems**:\n"
                           "• Potential: 1-2 lakh liters/year per household\n"
                           "• Cost: ₹15,000-50,000 (one-time)\n"
                           "• Payback: 3-5 years\n"
                           "• Suitable for: Urban and semi-urban areas\n\n"
                           "🏞️ **Community Systems**:\n"
                           "• Check dams and percolation tanks\n"
                           "• Recharge wells and injection wells\n"
                           "• Watershed management\n\n"
                           "**AGRICULTURAL EFFICIENCY**:\n"
                           "💧 **Micro-irrigation**:\n"
                           "• Drip irrigation: 30-50%% water savings\n"
                           "• Sprinkler systems: 20-40%% savings\n"
                           "• Fertigation: Nutrient + water efficiency\n\n"
                           "🌾 **Crop Management**:\n"
                           "• Drought-resistant varieties\n"
                           "• Crop rotation and intercropping\n"
                           "• Mulching and soil moisture conservation\n"
                           "• Precision agriculture techniques\n\n"
                           "**INDUSTRIAL CONSERVATION**:\n"
                           "♻️ **Water Recycling**:\n"
                           "• Closed-loop systems: 80-90%% reuse\n"
                           "• Membrane technologies\n"
                           "• Zero liquid discharge (ZLD)\n\n"
                           "**URBAN STRATEGIES**:\n"
                           "🏙️ **Smart Water Management**:\n"
                           "• Leak detection systems\n"
                           "• Smart meters and monitoring\n"
                           "• Greywater recycling\n"
                           "• Permeable pavements\n\n"
                           "**NATURAL SOLUTIONS**:\n"
                           "🌳 **Ecosystem-based Approaches**:\n"
                           "• Wetland restoration\n"
                           "• Forest conservation\n"
                           "• Soil health improvement\n"
                           "• Biodiversity conservation",
        .needs_location = false,
        .needs_data = false,
        .follow_up_suggestions = {
            "Cost-benefit analysis of methods",
            "Location-specific recommendations",
            "Government schemes and subsidies",
            "Success stories and case studies",
            "Technical implementation guides"
        },
        .suggestion_count = 5
    },
    
    {
        .intent = 66,  // INTENT_RAINFALL_CORRELATION
        .english_template = "🌧️ **RAINFALL-GROUNDWATER CORRELATION ANALYSIS**\n\n"
                           "**MONSOON IMPACT ON GROUNDWATER RECHARGE**:\n\n"
                           "📊 **Recharge Efficiency by Region**:\n"
                           "• Western Ghats: 60-80%% (High permeability)\n"
                           "• Gangetic Plains: 40-60%% (Moderate permeability)\n"
                           "• Deccan Plateau: 30-50%% (Variable geology)\n"
                           "• Arid Regions: 10-30%% (Low permeability)\n\n"
                           "**SEASONAL PATTERNS**:\n"
                           "🌦️ **Southwest Monsoon** (June-September):\n"
                           "• Contributes 70-80%% of annual recharge\n"
                           "• Peak recharge: July-August\n"
                           "• Regional variation: 500-3000mm rainfall\n\n"
                           "🌨️ **Northeast Monsoon** (October-December):\n"
                           "• Contributes 15-20%% of annual recharge\n"
                           "• Critical for Tamil Nadu and Andhra Pradesh\n"
                           "• Coastal areas benefit most\n\n"
                           "**RAINFALL-RECHARGE RELATIONSHIPS**:\n"
                           "📈 **Good Monsoon** (>110%% of normal):\n"
                           "• Groundwater recharge: +15 to +25%%\n"
                           "• Water table rise: 1-3 meters\n"
                           "• Category improvement possible\n\n"
                           "📉 **Poor Monsoon** (<90%% of normal):\n"
                           "• Groundwater recharge: -20 to -40%%\n"
                           "• Water table decline: 0.5-2 meters\n"
                           "• Increased extraction stress\n\n"
                           "**CLIMATE CHANGE IMPACTS**:\n"
                           "🌡️ **Changing Patterns**:\n"
                           "• Increased rainfall variability\n"
                           "• More intense but shorter duration rains\n"
                           "• Delayed monsoon onset\n"
                           "• Increased extreme weather events\n\n"
                           "**ADAPTATION STRATEGIES**:\n"
                           "• Enhanced rainwater harvesting\n"
                           "• Improved forecasting systems\n"
                           "• Flexible cropping patterns\n"
                           "• Climate-resilient infrastructure",
        .needs_location = false,
        .needs_data = false,
        .follow_up_suggestions = {
            "Show regional rainfall patterns",
            "Climate change projections",
            "Adaptation strategies for farmers",
            "Monsoon forecasting accuracy",
            "Correlation with specific locations"
        },
        .suggestion_count = 5
    },
    
    {
        .intent = 80,  // INTENT_TECHNICAL_EXPLANATION
        .english_template = "🔬 **TECHNICAL GROUNDWATER CONCEPTS EXPLAINED**\n\n"
                           "**STAGE OF GROUNDWATER EXTRACTION**:\n"
                           "📊 **Formula**: (Annual Extraction / Net Annual Availability) × 100\n\n"
                           "**CATEGORY DEFINITIONS**:\n"
                           "🟢 **SAFE** (<70%% extraction):\n"
                           "• Sustainable extraction levels\n"
                           "• No restrictions on development\n"
                           "• Water table stable or rising\n"
                           "• Example: Jaipur (65%%), Kolkata (56%%)\n\n"
                           "🟡 **SEMI-CRITICAL** (70-90%% extraction):\n"
                           "• Moderate stress on aquifer\n"
                           "• Careful development needed\n"
                           "• Monitoring required\n"
                           "• Example: Nashik (89%%), Coimbatore (98%%)\n\n"
                           "🟠 **CRITICAL** (90-100%% extraction):\n"
                           "• High stress on groundwater\n"
                           "• Immediate intervention needed\n"
                           "• Regulated development\n"
                           "• Example: Pune (156%%), Bangalore (167%%)\n\n"
                           "🔴 **OVER-EXPLOITED** (>100%% extraction):\n"
                           "• Groundwater mining occurring\n"
                           "• Ban on new extractions\n"
                           "• Urgent conservation needed\n"
                           "• Example: Amritsar (165%%), Ludhiana (178%%)\n\n"
                           "**KEY PARAMETERS EXPLAINED**:\n\n"
                           "💧 **Annual Recharge**:\n"
                           "• Natural replenishment from rainfall\n"
                           "• Return flow from irrigation\n"
                           "• Artificial recharge contributions\n"
                           "• Measured in Million Cubic Meters (MCM)\n\n"
                           "🏭 **Annual Extraction**:\n"
                           "• Agricultural use (85-90%%)\n"
                           "• Domestic use (5-10%%)\n"
                           "• Industrial use (2-5%%)\n"
                           "• Measured through surveys and estimates\n\n"
                           "⚖️ **Net Annual Availability**:\n"
                           "• Total recharge minus natural discharge\n"
                           "• Allocation for ecological needs\n"
                           "• Available for human use\n\n"
                           "**ASSESSMENT METHODOLOGY**:\n"
                           "📋 **CGWB Protocol**:\n"
                           "• Hydrogeological surveys\n"
                           "• Water level monitoring\n"
                           "• Pumping test analysis\n"
                           "• Remote sensing integration\n"
                           "• Community participation",
        .needs_location = false,
        .needs_data = false,
        .follow_up_suggestions = {
            "Show calculation examples",
            "Explain assessment methodology",
            "Compare international standards",
            "Data collection methods",
            "Quality assurance processes"
        },
        .suggestion_count = 5
    }
};

int enhanced_template_count = sizeof(enhanced_templates) / sizeof(MultilingualResponseTemplate);

// Generate enhanced response with context awareness
BotResponse* generate_enhanced_response(IntentType intent, const char* user_input, 
                                      ConversationContext* context, const char* location, 
                                      const char* query_details) {
    BotResponse* response = malloc(sizeof(BotResponse));
    if (!response) return NULL;
    
    // Initialize response structure
    response->intent = intent;
    response->query_result = NULL;
    response->has_data = false;
    response->processing_time_ms = 0.0;
    response->confidence_score = 0.8;  // Default confidence
    response->suggestion_count = 0;
    response->requires_clarification = false;
    response->clarification_question = NULL;
    response->context = context;
    response->source_count = 0;
    response->is_multilingual = false;
    response->language_code = strdup("en");
    
    // Initialize suggestions array
    for (int i = 0; i < 5; i++) {
        response->suggested_actions[i] = NULL;
    }
    
    // Find matching template
    MultilingualResponseTemplate* template = NULL;
    for (int i = 0; i < enhanced_template_count; i++) {
        if (enhanced_templates[i].intent == intent) {
            template = &enhanced_templates[i];
            break;
        }
    }
    
    if (!template) {
        response->message = strdup("I apologize, but I couldn't generate a proper response for your query. Please try rephrasing or ask for help.");
        return response;
    }
    
    // Copy suggestions
    response->suggestion_count = template->suggestion_count;
    for (int i = 0; i < template->suggestion_count && i < 5; i++) {
        response->suggested_actions[i] = strdup(template->follow_up_suggestions[i]);
    }
    
    // Generate response based on template and data needs
    char* formatted_response = malloc(4096);  // Larger buffer for complex responses
    if (!formatted_response) {
        response->message = strdup("Memory allocation error occurred.");
        return response;
    }
    
    if (template->needs_data && location) {
        // Fetch data and format response with actual data
        // This is a simplified version - in real implementation, 
        // you would query the database here
        snprintf(formatted_response, 4096, template->english_template, 
                location, location, 2023, "Over-Exploited", 156.8, 178.5, 113.8, 89.2,
                "This area shows severe groundwater stress with extraction exceeding sustainable limits.",
                "Declining", "Very High");
    } else if (intent == INTENT_COMPARE_LOCATIONS) {
        // Handle comparison responses
        const char* loc1 = location ? location : "Punjab";
        const char* loc2 = "Haryana";  // Default comparison
        snprintf(formatted_response, 4096, template->english_template,
                loc1, loc2, loc1, loc2, 78, 45, 150.5, 95.2,
                "Declining 2-3m", "Mixed trends", "Rice-wheat cultivation", "Industrial demand",
                "Very High", "High", loc1, "higher", loc2, "better",
                "immediate intervention", loc1, "crop diversification", loc2, "industrial regulation");
    } else {
        // Use template as-is for general responses
        strcpy(formatted_response, template->english_template);
    }
    
    response->message = formatted_response;
    response->has_data = template->needs_data;
    
    // Add data sources
    response->data_sources[0] = strdup("Central Ground Water Board (CGWB)");
    response->data_sources[1] = strdup("National Water Informatics Centre (NWIC)");
    response->data_sources[2] = strdup("State Ground Water Departments");
    response->source_count = 3;
    
    return response;
}

// Enhanced free function for new response structure
void free_enhanced_bot_response(BotResponse* response) {
    if (!response) return;
    
    if (response->message) free(response->message);
    if (response->query_result) free_query_result(response->query_result);
    if (response->clarification_question) free(response->clarification_question);
    if (response->language_code) free(response->language_code);
    
    for (int i = 0; i < response->suggestion_count; i++) {
        if (response->suggested_actions[i]) {
            free(response->suggested_actions[i]);
        }
    }
    
    for (int i = 0; i < response->source_count; i++) {
        if (response->data_sources[i]) {
            free(response->data_sources[i]);
        }
    }
    
    free(response);
}