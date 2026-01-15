CXX = g++

# --- OS Detection Logic ---
# Check if "Ubuntu" exists in /etc/os-release
IS_UBUNTU := $(shell grep -i ubuntu /etc/os-release 2>/dev/null)

ifeq ($(IS_UBUNTU),)
    GLFW_LIB = -lglfw
else
    GLFW_LIB = -lglfw3
endif
# --------------------------

CXXFLAGS = -std=c++20 -O2 -Wall -Wextra \
           -I./include -I./src \
           -I./external/imgui -I./external/imgui/backends \
           -I./external/tinyobjloader \
           -I./include/glad \
           -DIMGUI_IMPL_GLFW_DISABLE_CUSTOM_PLATFORM_CHECK -MP -MMD

# Use the dynamic GLFW_LIB variable here
LDFLAGS = $(GLFW_LIB) -lGL -lwayland-client -lxkbcommon -lpthread -ldl

SRCS = src/main.cpp src/platform/rendering/texture.cpp src/game/game.cpp \
       src/assets/assetManager.cpp src/util/utilStatics.cpp \
       include/glad/glad.c \
       external/imgui/imgui.cpp \
       external/imgui/imgui_draw.cpp \
       external/imgui/imgui_widgets.cpp \
       external/imgui/imgui_tables.cpp \
       external/imgui/imgui_demo.cpp \
       external/imgui/backends/imgui_impl_glfw.cpp \
       external/imgui/backends/imgui_impl_opengl3.cpp 

OBJS = $(addsuffix .o, $(basename $(SRCS)))
DEPS = $(OBJS:.o=.d)

all: app.out

app.out: $(OBJS)
	$(CXX) $(OBJS) -o app.out $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	gcc $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) app.out
