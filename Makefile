CXX      = g++
CXXFLAGS = -std=c++17 -Wall
-Wextra -g
LDFLAGS  = -pthread
SRC_DIR  = src
OBJ_DIR  = build
TARGET   = miniso
 
SRCS = $(wildcard
$(SRC_DIR)/*.cpp)
OBJS = $(patsubst
$(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
 
all: $(TARGET)
 
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
 
$(OBJ_DIR)/%.o:
$(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
 
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
 
run: $(TARGET)
	./$(TARGET)
 
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
 
.PHONY: all run clean
