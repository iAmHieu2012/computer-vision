CXX       := g++
CXX_FLAGS := -std=c++17 -ggdb
BIN       := bin
SRC       := src
INCLUDE   := include
BUILD     := build
EXECUTABLE := 23122027
CFLAGS    := $(shell pkg-config --cflags opencv4)
LIBRARIES := $(shell pkg-config --libs opencv4)
TARGET    := $(BIN)/$(EXECUTABLE)

SRCS := $(wildcard $(SRC)/*.cpp)
OBJS := $(patsubst $(SRC)/%.cpp, $(BUILD)/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

all: $(TARGET)

run: clean all
	clear
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LIBRARIES)

$(BUILD)/%.o: $(SRC)/%.cpp
	@mkdir -p $(BUILD)
	$(CXX) $(CXX_FLAGS) $(CFLAGS) -I$(INCLUDE) -MMD -MP -c $< -o $@

clean:
	-rm -rf $(BUILD) $(BIN)/*

-include $(DEPS)