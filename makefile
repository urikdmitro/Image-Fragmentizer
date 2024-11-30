CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -g

IMGUI_DIR       := ./vendor/imgui
NFD_DIR         := ./vendor/nativefiledialog
STB_DIR         := ./vendor/stb
GLFW_DIR        := /usr
GLEW_DIR        := /usr
GTK_FLAGS       := `pkg-config --cflags --libs gtk+-3.0`
BUILD_DIR       := ./build
SRC_DIR         := ./src
INCLUDES_DIR    := ./include

INCLUDES := -I$(IMGUI_DIR) \
            -I$(IMGUI_DIR)/backends \
            -I$(GLFW_DIR)/include \
            -I$(GLEW_DIR)/include \
            -I$(NFD_DIR)/src/include \
            -I$(STB_DIR)/ \
            -I$(INCLUDES_DIR)/ \
            -I$(INCLUDES_DIR)/model \
            -I$(INCLUDES_DIR)/view \
            -I$(INCLUDES_DIR)/controller

LIBS := -L$(GLFW_DIR)/lib \
        -L$(GLEW_DIR)/lib \
        -L$(NFD_DIR)/build/lib/Release/x64 \
        -L$(IMGUI_DIR)/backends \
        -L$(IMGUI_DIR) \
        -lglfw \
        -lGLEW \
        -lGL \
        -lnfd

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) \
             $(wildcard $(SRC_DIR)/model/*.cpp) \
             $(wildcard $(SRC_DIR)/view/*.cpp) \
             $(wildcard $(SRC_DIR)/controller/*.cpp) \
             $(IMGUI_DIR)/imgui.cpp \
             $(IMGUI_DIR)/imgui_draw.cpp \
             $(IMGUI_DIR)/imgui_tables.cpp \
             $(IMGUI_DIR)/imgui_widgets.cpp \
             $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
             $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

OBJS := $(SRC_FILES:%.cpp=$(BUILD_DIR)/%.o)

TARGET := app

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(GTK_FLAGS) $(OBJS) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@) # Ensure build directory structure
	$(CXX) $(CXXFLAGS) $(GTK_FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/src $(TARGET)

hard_clean:
	rm -rf $(BUILD_DIR) $(TARGET)
