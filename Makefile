# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = -lgen # For basename/dirname

# Directories
SRC_DIR = src
BUILD_DIR = build
DIST_DIR = dist
BIN_DIR = $(DIST_DIR)/bin
SCRIPTS_DIR_REPO = scripts
SCRIPTS_DIR_DIST = $(DIST_DIR)/scripts

# Executable name
TARGET_EXEC = clz4
TARGET = $(BUILD_DIR)/$(TARGET_EXEC)

# Source files
C_SOURCES = $(SRC_DIR)/clz4.c

# Object files
OBJS = $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Target to create distribution directories
$(DIST_DIR):
	mkdir -p $(DIST_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(SCRIPTS_DIR_DIST)

install: all | $(DIST_DIR)
	@echo "Installing $(TARGET_EXEC) to $(BIN_DIR)..."
	cp $(TARGET) $(BIN_DIR)/
	@echo "Installing scripts to $(SCRIPTS_DIR_DIST)..."
	cp $(SCRIPTS_DIR_REPO)/* $(SCRIPTS_DIR_DIST)/
	@echo ""
	@echo "Installation complete."
	@echo "You can add $(abspath $(BIN_DIR)) and $(abspath $(SCRIPTS_DIR_DIST)) to your PATH,"
	@echo "or copy the files to a directory in your PATH (e.g., /usr/local/bin/)."
	@echo "Example to copy to /usr/local/bin (requires sudo):"
	@echo "  sudo cp $(BIN_DIR)/$(TARGET_EXEC) /usr/local/bin/"
	@echo "  sudo cp $(SCRIPTS_DIR_DIST)/* /usr/local/bin/"


uninstall:
	@echo "Removing installed files from $(BIN_DIR) and $(SCRIPTS_DIR_DIST)..."
	rm -f $(BIN_DIR)/$(TARGET_EXEC)
	rm -f $(SCRIPTS_DIR_DIST)/*
	@if [ -d "$(SCRIPTS_DIR_DIST)" ]; then rmdir --ignore-fail-on-non-empty $(SCRIPTS_DIR_DIST); fi
	@if [ -d "$(BIN_DIR)" ]; then rmdir --ignore-fail-on-non-empty $(BIN_DIR); fi
	@if [ -d "$(DIST_DIR)" ]; then rmdir --ignore-fail-on-non-empty $(DIST_DIR); fi
	@echo "Uninstall complete from repository's dist directory."

clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)
	@echo "Cleaning distribution directory..."
	rm -rf $(DIST_DIR)
	@echo "Clean complete."
