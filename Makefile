CC = clang++
CFLAGS = -std=c++20 -Wall
LDFLAGS = -L/usr/local/lib -luv
INCLUDES = -I/usr/local/include

BUILD_DIR = build
SOURCES = main.cpp
OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))

EXECUTABLE = $(BUILD_DIR)/libuv-modules

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

