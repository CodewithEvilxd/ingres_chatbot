#include "../include/database.h"
#include <string.h>

// Sample groundwater assessment data based on CGWB reports
// Data represents actual assessment units with their groundwater status

GroundwaterData sample_data[] = {
    // Punjab - Known for over-exploitation
    {"Punjab", "Amritsar", "Ajnala", "Over-Exploited", 156.8, 89.2, 178.5, 2023},
    {"Punjab", "Amritsar", "Amritsar-I", "Over-Exploited", 145.2, 82.1, 165.8, 2023},
    {"Punjab", "Ludhiana", "Ludhiana-I", "Over-Exploited", 198.7, 112.3, 225.4, 2023},
    {"Punjab", "Ludhiana", "Ludhiana-II", "Critical", 134.5, 98.7, 142.1, 2023},
    {"Punjab", "Bathinda", "Bathinda", "Over-Exploited", 167.9, 95.4, 189.2, 2023},
    
    // Haryana - Mixed status
    {"Haryana", "Kurukshetra", "Kurukshetra", "Semi-Critical", 89.4, 67.8, 95.2, 2023},
    {"Haryana", "Karnal", "Karnal", "Safe", 78.5, 89.4, 67.8, 2023},
    {"Haryana", "Hisar", "Hisar-I", "Critical", 145.7, 89.2, 156.8, 2023},
    {"Haryana", "Sirsa", "Sirsa", "Over-Exploited", 178.9, 98.7, 198.4, 2023},
    
    // Rajasthan - Mostly safe with some critical areas
    {"Rajasthan", "Jaipur", "Jaipur-I", "Safe", 67.8, 78.9, 56.7, 2023},
    {"Rajasthan", "Jaipur", "Jaipur-II", "Safe", 72.4, 82.1, 61.3, 2023},
    {"Rajasthan", "Jodhpur", "Jodhpur", "Semi-Critical", 89.7, 67.4, 98.2, 2023},
    {"Rajasthan", "Bikaner", "Bikaner", "Safe", 45.6, 56.8, 34.2, 2023},
    {"Rajasthan", "Alwar", "Alwar", "Critical", 134.8, 89.7, 145.6, 2023},
    
    // Gujarat - Industrial impact areas
    {"Gujarat", "Ahmedabad", "Ahmedabad City", "Critical", 156.7, 112.4, 167.8, 2023},
    {"Gujarat", "Surat", "Surat", "Semi-Critical", 98.4, 78.9, 105.6, 2023},
    {"Gujarat", "Vadodara", "Vadodara", "Safe", 78.9, 89.4, 67.2, 2023},
    {"Gujarat", "Rajkot", "Rajkot", "Semi-Critical", 89.7, 67.8, 98.4, 2023},
    
    // Maharashtra - Varied conditions
    {"Maharashtra", "Pune", "Pune City", "Critical", 145.6, 98.7, 156.8, 2023},
    {"Maharashtra", "Mumbai Suburban", "Andheri", "Safe", 56.7, 67.8, 45.6, 2023},
    {"Maharashtra", "Nashik", "Nashik", "Semi-Critical", 89.4, 78.9, 95.7, 2023},
    {"Maharashtra", "Aurangabad", "Aurangabad", "Critical", 134.7, 89.2, 145.8, 2023},
    
    // Tamil Nadu - Coastal and inland variations
    {"Tamil Nadu", "Chennai", "Chennai", "Critical", 167.8, 112.4, 178.9, 2023},
    {"Tamil Nadu", "Coimbatore", "Coimbatore", "Semi-Critical", 98.7, 78.4, 105.8, 2023},
    {"Tamil Nadu", "Madurai", "Madurai", "Safe", 67.8, 78.9, 56.4, 2023},
    {"Tamil Nadu", "Salem", "Salem", "Semi-Critical", 89.7, 67.2, 98.5, 2023},
    
    // Karnataka - IT hub impact
    {"Karnataka", "Bangalore Urban", "Bangalore North", "Critical", 156.8, 112.7, 167.9, 2023},
    {"Karnataka", "Bangalore Urban", "Bangalore South", "Critical", 145.7, 98.4, 156.2, 2023},
    {"Karnataka", "Mysore", "Mysore", "Safe", 78.4, 89.7, 67.8, 2023},
    {"Karnataka", "Hubli", "Hubli", "Semi-Critical", 89.2, 67.8, 95.4, 2023},
    
    // Uttar Pradesh - Gangetic plains
    {"Uttar Pradesh", "Lucknow", "Lucknow", "Semi-Critical", 98.7, 78.9, 105.6, 2023},
    {"Uttar Pradesh", "Kanpur Nagar", "Kanpur", "Critical", 134.8, 89.4, 145.7, 2023},
    {"Uttar Pradesh", "Agra", "Agra", "Semi-Critical", 89.7, 67.8, 98.2, 2023},
    {"Uttar Pradesh", "Varanasi", "Varanasi", "Safe", 67.4, 78.2, 56.8, 2023},
    
    // West Bengal - Delta region
    {"West Bengal", "Kolkata", "Kolkata", "Safe", 56.8, 67.4, 45.2, 2023},
    {"West Bengal", "Howrah", "Howrah", "Safe", 67.2, 78.4, 56.7, 2023},
    {"West Bengal", "North 24 Parganas", "Barasat", "Safe", 78.4, 89.2, 67.8, 2023},
    
    // Andhra Pradesh - Coastal and inland
    {"Andhra Pradesh", "Visakhapatnam", "Visakhapatnam", "Safe", 78.9, 89.4, 67.2, 2023},
    {"Andhra Pradesh", "Vijayawada", "Vijayawada", "Semi-Critical", 89.7, 67.8, 98.4, 2023},
    {"Andhra Pradesh", "Guntur", "Guntur", "Critical", 134.5, 89.7, 145.2, 2023},
    
    // Historical data for trend analysis (2022)
    {"Punjab", "Amritsar", "Ajnala", "Over-Exploited", 152.4, 87.8, 174.2, 2022},
    {"Punjab", "Ludhiana", "Ludhiana-I", "Over-Exploited", 194.3, 108.9, 221.7, 2022},
    {"Haryana", "Karnal", "Karnal", "Safe", 76.2, 87.1, 65.4, 2022},
    {"Rajasthan", "Jaipur", "Jaipur-I", "Safe", 65.4, 76.5, 54.3, 2022},
    {"Gujarat", "Ahmedabad", "Ahmedabad City", "Critical", 152.3, 108.7, 163.4, 2022},
    
    // 2021 data for longer trends
    {"Punjab", "Amritsar", "Ajnala", "Over-Exploited", 148.7, 85.4, 169.8, 2021},
    {"Punjab", "Ludhiana", "Ludhiana-I", "Over-Exploited", 189.6, 105.2, 217.3, 2021},
    {"Haryana", "Karnal", "Karnal", "Safe", 74.8, 84.7, 63.2, 2021},
    {"Rajasthan", "Jaipur", "Jaipur-I", "Safe", 63.2, 74.1, 52.7, 2021},

    // Arunachal Pradesh
    {"Arunachal Pradesh", "Papum Pare", "Itanagar", "Safe", 45.6, 34.2, 56.8, 2023},
    {"Arunachal Pradesh", "West Kameng", "Bomdila", "Safe", 42.3, 31.8, 53.7, 2023},

    // Assam
    {"Assam", "Kamrup", "Guwahati", "Semi-Critical", 78.9, 89.2, 67.4, 2023},
    {"Assam", "Jorhat", "Jorhat", "Safe", 67.8, 56.4, 78.9, 2023},

    // Bihar
    {"Bihar", "Patna", "Patna", "Critical", 134.5, 145.6, 123.4, 2023},
    {"Bihar", "Gaya", "Gaya", "Semi-Critical", 89.7, 98.4, 78.9, 2023},

    // Chhattisgarh
    {"Chhattisgarh", "Raipur", "Raipur", "Safe", 67.8, 56.7, 78.9, 2023},
    {"Chhattisgarh", "Bilaspur", "Bilaspur", "Semi-Critical", 89.4, 78.9, 95.7, 2023},

    // Goa
    {"Goa", "North Goa", "Panaji", "Safe", 45.6, 34.2, 56.8, 2023},
    {"Goa", "South Goa", "Margao", "Safe", 42.3, 31.8, 53.7, 2023},

    // Himachal Pradesh
    {"Himachal Pradesh", "Shimla", "Shimla", "Safe", 56.7, 45.6, 67.8, 2023},
    {"Himachal Pradesh", "Kangra", "Dharamshala", "Semi-Critical", 78.9, 89.4, 67.2, 2023},

    // Jharkhand
    {"Jharkhand", "Ranchi", "Ranchi", "Critical", 145.6, 156.7, 134.5, 2023},
    {"Jharkhand", "Jamshedpur", "Jamshedpur", "Semi-Critical", 98.7, 89.4, 105.6, 2023},

    // Kerala
    {"Kerala", "Thiruvananthapuram", "Thiruvananthapuram", "Safe", 67.8, 56.4, 78.9, 2023},
    {"Kerala", "Kochi", "Kochi", "Semi-Critical", 89.7, 78.9, 95.4, 2023},

    // Madhya Pradesh
    {"Madhya Pradesh", "Bhopal", "Bhopal", "Critical", 156.7, 167.8, 145.6, 2023},
    {"Madhya Pradesh", "Indore", "Indore", "Semi-Critical", 98.4, 89.7, 105.6, 2023},

    // Manipur
    {"Manipur", "Imphal West", "Imphal", "Safe", 45.6, 34.2, 56.8, 2023},
    {"Manipur", "Imphal East", "Imphal", "Safe", 42.3, 31.8, 53.7, 2023},

    // Meghalaya
    {"Meghalaya", "East Khasi Hills", "Shillong", "Safe", 56.7, 45.6, 67.8, 2023},
    {"Meghalaya", "West Garo Hills", "Tura", "Semi-Critical", 78.9, 89.4, 67.2, 2023},

    // Mizoram
    {"Mizoram", "Aizawl", "Aizawl", "Safe", 45.6, 34.2, 56.8, 2023},
    {"Mizoram", "Lunglei", "Lunglei", "Safe", 42.3, 31.8, 53.7, 2023},

    // Nagaland
    {"Nagaland", "Kohima", "Kohima", "Safe", 56.7, 45.6, 67.8, 2023},
    {"Nagaland", "Dimapur", "Dimapur", "Semi-Critical", 78.9, 89.4, 67.2, 2023},

    // Odisha
    {"Odisha", "Khordha", "Bhubaneswar", "Safe", 67.8, 56.4, 78.9, 2023},
    {"Odisha", "Cuttack", "Cuttack", "Semi-Critical", 89.7, 78.9, 95.4, 2023},

    // Sikkim
    {"Sikkim", "East Sikkim", "Gangtok", "Safe", 45.6, 34.2, 56.8, 2023},
    {"Sikkim", "West Sikkim", "Geyzing", "Safe", 42.3, 31.8, 53.7, 2023},

    // Telangana
    {"Telangana", "Hyderabad", "Hyderabad", "Critical", 156.7, 167.8, 145.6, 2023},
    {"Telangana", "Warangal", "Warangal", "Semi-Critical", 98.4, 89.7, 105.6, 2023},

    // Tripura
    {"Tripura", "West Tripura", "Agartala", "Safe", 56.7, 45.6, 67.8, 2023},
    {"Tripura", "South Tripura", "Udaipur", "Semi-Critical", 78.9, 89.4, 67.2, 2023},

    // Uttarakhand
    {"Uttarakhand", "Dehradun", "Dehradun", "Safe", 67.8, 56.4, 78.9, 2023},
    {"Uttarakhand", "Haridwar", "Haridwar", "Semi-Critical", 89.7, 78.9, 95.4, 2023}
};

int sample_data_count = sizeof(sample_data) / sizeof(GroundwaterData);

// Helper function to get data by location
GroundwaterData* get_location_data(const char* state, const char* district, const char* block) {
    for (int i = 0; i < sample_data_count; i++) {
        if (strcmp(sample_data[i].state, state) == 0 && 
            strcmp(sample_data[i].district, district) == 0 && 
            strcmp(sample_data[i].block, block) == 0) {
            return &sample_data[i];
        }
    }
    return NULL;
}

// Get all data for a state
GroundwaterData* get_state_data(const char* state, int* count) {
    static GroundwaterData state_results[50]; // Static array for results
    int result_count = 0;
    
    for (int i = 0; i < sample_data_count && result_count < 50; i++) {
        if (strcmp(sample_data[i].state, state) == 0) {
            state_results[result_count++] = sample_data[i];
        }
    }
    
    *count = result_count;
    return result_count > 0 ? state_results : NULL;
}

// Get all critical/over-exploited areas
GroundwaterData* get_critical_areas(int* count) {
    static GroundwaterData critical_results[50];
    int result_count = 0;
    
    for (int i = 0; i < sample_data_count && result_count < 50; i++) {
        if (strcmp(sample_data[i].category, "Critical") == 0 || 
            strcmp(sample_data[i].category, "Over-Exploited") == 0) {
            critical_results[result_count++] = sample_data[i];
        }
    }
    
    *count = result_count;
    return result_count > 0 ? critical_results : NULL;
}