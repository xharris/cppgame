# TARGET ?= game.exe

# SRC_DIRS ?= ./src

# SRCS := $(shell Get-ChildItem $(SRC_DIRS) -Filter *.cpp | Select FullName)# find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
# OBJS := $(addsuffix .o,$(basename $(SRCS)))

NOTMAIN = engine.cpp graphics.cpp window.cpp ecs_graph.cpp
NOTMAIN := $(addprefix src/,$(NOTMAIN))
OBJS = src/main.cpp $(NOTMAIN)
OBJS_TEST = src/test.cpp $(NOTMAIN)
OUT = game.exe
OUT_TEST = test.exe

CC = g++

# SDL
# SDL_ARCH = x86_64-w64-mingw32
# INCLUDE = -I./libs/SDL2/$(SDL_ARCH)/include/SDL2 \
# 					-I./libs/SDL2_image/$(SDL_ARCH)/include/SDL2
# LIB = 		-L./libs/SDL2/$(SDL_ARCH)/lib \
# 					-L./libs/SDL2_image/$(SDL_ARCH)/lib
# LINK = 		-lmingw32 -lSDL2main -lSDL2 -lSDL2_image

# RAYLIB
RAYLIB_ARCH =	raylib-3.7.0_win64_mingw-w64
INCLUDE =	-I./libs/$(RAYLIB_ARCH)/include
LIB = -L./libs/$(RAYLIB_ARCH)/lib
LINK = -lraylib -lopengl32 -lgdi32 -lwinmm

# LUA
INCLUDE += -I./libs/lua-5.4/include
LIB += -L./libs/lua-5.4
LINK += -llua54

# SOL2
INCLUDE += -I./libs/sol2

# TESTING
# INCLUDE += -I./libs/catch2
INCLUDE += -I./libs/doctest

# SOLE (uuids)
# INCLUDE += -I./libs/sole-1.0.1

CPP_FLAGS = -std=c++17
ifeq ($(VERBOSE), 1)
	CPP_FLAGS += -v
endif

all: $(OBJS)
	-clean
	$(CC) $(OBJS) $(INCLUDE) $(LIB) $(CPP_FLAGS) $(LINK) -o $(OUT)
	$(OUT)

test: $(OBJS_TEST)
	-clean
	$(CC) $(OBJS_TEST) $(INCLUDE) $(LIB) $(CPP_FLAGS) $(LINK) -o $(OUT_TEST)
	$(OUT_TEST)

run:
	$(OUT)

clean: $(OUT)
	$(RM) $(OUT)
	$(RM) $(OUT_TEST)

ifeq ($(OS),Windows_NT)
    RM = cmd /C del /Q /F
    RRM = cmd /C rmdir /Q /S
else
    RM = rm -f
    RRM = rm -f -r
endif