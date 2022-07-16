targetName := main

target := $(targetName)
ifeq ($(OS), Windows_NT)
	target := $(target).exe
endif

CXX := g++
LDFLAGS := -lgdi32 -lopengl32
CPPFLAGS := -Wall
CXXFLAGS := -std=gnu++17

objFolder := ./obj
srcFolder := ./src
binFolder := ./bin

SRCS := main.cpp win32window.cpp
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

.PHONY: run

run: $(binFolder)/$(target)
	$(binFolder)/$(target)

$(binFolder)/$(target): $(addprefix $(objFolder)/,$(OBJS))
	$(CXX) $^ -o $@ $(LDFLAGS)

$(objFolder)/%.o: $(srcFolder)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@