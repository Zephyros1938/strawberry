CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -I./include -I./src -I./src/external -DIMGUI_IMPL_GLFW_DISABLE_CUSTOM_PLATFORM_CHECK -MP -MMD
LDFLAGS = -lglfw3 -lGL -lwayland-client -lxkbcommon -lpthread -ldl -lstb

# List your source files here
SRCS = src/main.cpp src/game.cpp src/texture.cpp src/assetManager.cpp \
       src/glad/glad.c src/external/imgui.cpp src/external/imgui_draw.cpp \
       src/external/imgui_widgets.cpp src/external/imgui_tables.cpp \
       src/external/imgui_demo.cpp src/external/imgui_impl_glfw.cpp \
       src/external/imgui_impl_opengl3.cpp

# Convert source file names to object file names (.o)
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)
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
