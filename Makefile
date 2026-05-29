BUILD_DIR := build
BINARY    := $(BUILD_DIR)/StarWarsGame

.PHONY: all build run clean rebuild debug release

all: build

# Configure + compile (debug by default)
build:
	@cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -Wno-dev > /dev/null
	@cmake --build $(BUILD_DIR) --parallel

# Configure + compile in release mode
release:
	@cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -Wno-dev > /dev/null
	@cmake --build $(BUILD_DIR) --parallel

# Build then run
run: build
	@./$(BINARY)

# Just run whatever was last built
play:
	@./$(BINARY)

# Full clean rebuild
rebuild: clean build

# Remove build artifacts
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned."

# Show which SDL2 libraries were found
deps:
	@cmake -B $(BUILD_DIR) -Wno-dev 2>&1 | grep -E "SDL2|found|not found"
