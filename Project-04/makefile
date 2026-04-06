# Source and target
SRCS = main.cpp
TARGET = raylib_app

# Add the CS3113 library if it exists
ifeq ($(wildcard CS3113/cs3113.cpp),CS3113/cs3113.cpp)
    SRCS += CS3113/cs3113.cpp
endif

# Add the Map library if it exists
ifeq ($(wildcard CS3113/Map.cpp),CS3113/Map.cpp)
    SRCS += CS3113/Map.cpp
endif

# Add the Entity library if it exists
ifeq ($(wildcard CS3113/Entity.cpp),CS3113/Entity.cpp)
    SRCS += CS3113/Entity.cpp
endif

# Add the Player library if it exists
ifeq ($(wildcard CS3113/Player.cpp),CS3113/Player.cpp)
    SRCS += CS3113/Player.cpp
endif

# Add the Enemy library if it exists
ifeq ($(wildcard CS3113/Enemy.cpp),CS3113/Enemy.cpp)
    SRCS += CS3113/Enemy.cpp
endif

# Add the Wanderer library if it exists
ifeq ($(wildcard CS3113/Wanderer.cpp),CS3113/Wanderer.cpp)
    SRCS += CS3113/Wanderer.cpp
endif

# Add the Follower library if it exists
ifeq ($(wildcard CS3113/Follower.cpp),CS3113/Follower.cpp)
    SRCS += CS3113/Follower.cpp
endif

# Add the Jumper library if it exists
ifeq ($(wildcard CS3113/Jumper.cpp),CS3113/Jumper.cpp)
    SRCS += CS3113/Jumper.cpp
endif

# Add the Scene library if it exists
ifeq ($(wildcard CS3113/Scene.cpp),CS3113/Scene.cpp)
    SRCS += CS3113/Scene.cpp
endif

# Add the LevelA library if it exists
ifeq ($(wildcard CS3113/LevelA.cpp),CS3113/LevelA.cpp)
    SRCS += CS3113/LevelA.cpp
endif

# Add the LevelB library if it exists
ifeq ($(wildcard CS3113/LevelB.cpp),CS3113/LevelB.cpp)
    SRCS += CS3113/LevelB.cpp
endif

# Add the LevelC library if it exists
ifeq ($(wildcard CS3113/LevelC.cpp),CS3113/LevelC.cpp)
    SRCS += CS3113/LevelC.cpp
endif

ifeq ($(wildcard CS3113/Life.cpp),CS3113/Life.cpp)
    SRCS += CS3113/Life.cpp
endif

ifeq ($(wildcard CS3113/Flagpole.cpp),CS3113/Flagpole.cpp)
    SRCS += CS3113/Flagpole.cpp
endif

ifeq ($(wildcard CS3113/TitleScene.cpp),CS3113/TitleScene.cpp)
    SRCS += CS3113/TitleScene.cpp
endif

ifeq ($(wildcard CS3113/WinScene.cpp),CS3113/WinScene.cpp)
    SRCS += CS3113/WinScene.cpp
endif

ifeq ($(wildcard CS3113/DeathScene.cpp),CS3113/DeathScene.cpp)
    SRCS += CS3113/DeathScene.cpp
endif

# OS detection (macOS = Darwin, Windows via MinGW = MINGW*)
UNAME_S := $(shell uname -s)

# Default values
CXX = g++
CXXFLAGS = -std=c++11

# Raylib configuration using pkg-config
RAYLIB_CFLAGS = $(shell pkg-config --cflags raylib)
RAYLIB_LIBS = $(shell pkg-config --libs raylib)

ifeq ($(UNAME_S), Darwin)
    # macOS configuration
    CXXFLAGS += -arch arm64 $(RAYLIB_CFLAGS)
    LIBS = $(RAYLIB_LIBS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    EXEC = ./$(TARGET)
else ifneq (,$(filter MINGW% MSYS% CYGWIN%,$(UNAME_S)))
    # Windows configuration (assumes raylib in C:/raylib)
    CXXFLAGS += -IC:/raylib/include
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
    EXEC = ./$(TARGET)
else
    # Linux/WSL fallback
    CXXFLAGS +=
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EXEC = ./$(TARGET)
endif

# Build rule
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# Clean rule
clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(TARGET).exe" ]; then rm -f $(TARGET).exe; fi

# Run rule
run: $(TARGET)
	$(EXEC)