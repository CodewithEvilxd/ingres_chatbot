cmake_minimum_required(VERSION 3.10)
project(ingres_chatbot C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add include directories
include_directories(include lib)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSON_C REQUIRED json-c)
pkg_check_modules(LIBPQ REQUIRED libpq)

# Define the executable with all source files
add_executable(ingres_chatbot
        src/main.c
        src/chatbot.c
        src/database.c
        src/api.c
        src/utils.c
        src/intent_patterns.c
        src/enhanced_intent_patterns.c
        src/enhanced_response_generator.c
        lib/mongoose.c
)

# Define the test suite executable
add_executable(test_suite
        src/test_suite.c
        src/chatbot.c
        src/database.c
        src/api.c
        src/utils.c
        src/intent_patterns.c
        src/enhanced_intent_patterns.c
        src/enhanced_response_generator.c
        lib/mongoose.c
)

# Include directories for found packages
target_include_directories(ingres_chatbot PRIVATE
        ${JSON_C_INCLUDE_DIRS}
        ${LIBPQ_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(ingres_chatbot
        m
        ws2_32
        ${JSON_C_LIBRARIES}
        ${LIBPQ_LIBRARIES}
)

# Add compile definitions
target_compile_definitions(ingres_chatbot PRIVATE
        ${JSON_C_CFLAGS_OTHER}
        ${LIBPQ_CFLAGS_OTHER}
)

# Apply same settings to test suite
target_include_directories(test_suite PRIVATE
        ${JSON_C_INCLUDE_DIRS}
        ${LIBPQ_INCLUDE_DIRS}
)

target_link_libraries(test_suite
        m
        ws2_32
        ${JSON_C_LIBRARIES}
        ${LIBPQ_LIBRARIES}
)

target_compile_definitions(test_suite PRIVATE
        ${JSON_C_CFLAGS_OTHER}
        ${LIBPQ_CFLAGS_OTHER}
)

# Set compiler flags for better performance and warnings
if(MSVC)
    target_compile_options(ingres_chatbot PRIVATE /W4 /O2)
    target_compile_options(test_suite PRIVATE /W4 /O2)
else()
    target_compile_options(ingres_chatbot PRIVATE -Wall -Wextra -Wpedantic -O2 -g)
    target_compile_options(test_suite PRIVATE -Wall -Wextra -Wpedantic -O2 -g)
endif()

# Create build directory structure
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(ingres_chatbot PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(test_suite PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)