CXX = g++
# Added new include paths for the submodules
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra \
           -I./include -I./src \
           -I./external/imgui -I./external/imgui/backends \
           -I./external/tinyobjloader \
					 -I./include/glad \
           -DIMGUI_IMPL_GLFW_DISABLE_CUSTOM_PLATFORM_CHECK -MP -MMD

LDFLAGS = -lglfw -lGL -lwayland-client -lxkbcommon -lpthread -ldl

# Updated SRCS to point to the new submodule locations
SRCS = src/main.cpp src/game.cpp src/texture.cpp src/assetManager.cpp \
       include/glad/glad.c \
       external/imgui/imgui.cpp \
       external/imgui/imgui_draw.cpp \
       external/imgui/imgui_widgets.cpp \
       external/imgui/imgui_tables.cpp \
       external/imgui/imgui_demo.cpp \
       external/imgui/backends/imgui_impl_glfw.cpp \
       external/imgui/backends/imgui_impl_opengl3.cpp

# Convert source file names to object file names (.o)
OBJS = $(addsuffix .o, $(basename $(SRCS)))
DEPS = $(OBJS:.o=.d)

all: app

app: $(OBJS)
	$(CXX) $(OBJS) -o app $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	gcc $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) app
