CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2 -g -Iinclude -Ilib
LDFLAGS = -lm -ljson-c -lpq -lssl -lcrypto
SRCDIR = src
INCDIR = include
LIBDIR = lib
OBJDIR = obj
BINDIR = bin

# Source files - using real implementations, not stubs
SOURCES = $(SRCDIR)/main.c \
          $(SRCDIR)/chatbot.c \
          $(SRCDIR)/database.c \
          $(SRCDIR)/api.c \
          $(SRCDIR)/utils.c \
          $(SRCDIR)/intent_patterns.c \
          $(SRCDIR)/enhanced_intent_patterns.c \
          $(SRCDIR)/enhanced_response_generator.c \
          $(LIBDIR)/mongoose.c

OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJECTS := $(OBJECTS:$(LIBDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/ingres_chatbot

# Default target
all: directories check-deps $(TARGET)

# Check for required dependencies
check-deps:
	@echo "🔍 Checking dependencies..."
	@command -v pkg-config >/dev/null 2>&1 || (echo "❌ pkg-config not found. Install with: sudo apt-get install pkg-config" && exit 1)
	@pkg-config --exists json-c || (echo "❌ json-c not found. Install with: sudo apt-get install libjson-c-dev" && exit 1)
	@pkg-config --exists libpq || (echo "❌ libpq not found. Install with: sudo apt-get install libpq-dev" && exit 1)
	@echo "✅ All dependencies found!"

# Create directories
directories:
	@mkdir -p $(OBJDIR) $(BINDIR)

# Link the executable
$(TARGET): $(OBJECTS)
	@echo "🔗 Linking $(TARGET)..."
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "✅ Build successful!"

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "📦 Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(LIBDIR)/%.c
	@echo "📦 Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	@echo "🧹 Cleaning build files..."
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "✅ Clean complete!"

# Clean and rebuild
rebuild: clean all

# Run the program
run: $(TARGET)
	@echo "🚀 Running INGRES ChatBot..."
	./$(TARGET)

# Run with server mode
server: $(TARGET)
	@echo "🌐 Starting API server..."
	./$(TARGET) --server --port 8080

# Test compilation only
test-compile: directories check-deps $(OBJECTS)
	@echo "✅ Compilation successful!"
	@echo "📊 Build statistics:"
	@wc -l $(SOURCES) | tail -1 | awk '{print "   • Total lines of code: " $$1}'
	@ls -lh $(TARGET) | awk '{print "   • Executable size: " $$5}'

# Debug build
debug: CFLAGS += -DDEBUG -O0
debug: clean all

# Performance profiling build
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: clean all

# Static analysis
analyze: CFLAGS += -fanalyzer
analyze: clean all

.PHONY: all clean rebuild run server test-compile debug profile analyze check-deps directories