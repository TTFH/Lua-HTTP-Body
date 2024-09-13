TARGET = server

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -Wpedantic -O3 -g
LIBS = `pkg-config --libs libcurl --static`
LIBS += -Llua5.1.4 -llua5.1

SOURCES = main.cpp
SOURCES += src/client.cpp src/server.cpp src/lua_utils.cpp

OBJDIR = obj
OBJS = $(SOURCES:.cpp=.o)
OBJS := $(OBJS:.c=.o)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(OBJS)))

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	ECHO_MESSAGE = "Linux"
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lWs2_32 -lwsock32
endif

ifeq ($(UNAME_S), Darwin)
	ECHO_MESSAGE = "MacOS"
endif

.PHONY: all clean rebuild

all: $(TARGET)
	@echo Build complete for $(ECHO_MESSAGE)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: src/%.cpp src/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

rebuild: clean all

clean:
	rm -f $(TARGET) $(OBJDIR)/*.o
