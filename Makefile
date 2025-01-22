SC_PATH = $(SYSTEMC_HOME)
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

TARGET = $(BUILD_DIR)/cache
CXX = g++
CXXFLAGS = -std=c++11 -DSC_DISABLE_API_VERSION_CHECK -I$(SC_PATH)/include
LDFLAGS = -L$(SC_PATH)/lib -lsystemc -lm

CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
C_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(CPP_FILES:.cpp=.o) $(C_FILES:.c=.o)))

TARGET = $(BUILD_DIR)/simulation

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean