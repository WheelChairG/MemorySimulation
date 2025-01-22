SC_PATH = $(SYSTEMC_HOME)
SRC_DIR = src
BUILD_DIR = build
SOURCES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(STC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
OBJECTS += $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
DEPS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(SOURCES))
DEPS += $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.d, $(SOURCES))
TARGET = $(BUILD_DIR)/cache
CXX = g++
CXXFLAGS = -std=c++11 -DSC_DISABLE_API_VERSION_CHECK -I$(SC_PATH)/include
LDFLAGS = -L$(SC_PATH)/lib -lsystemc -lm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	gcc $(CXXFLAGS) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR)

