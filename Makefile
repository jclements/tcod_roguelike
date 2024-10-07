# Define compiler and flags
CXX = g++
CXXFLAGS = -Iinclude -Wall -g
LDFLAGS = -L. -ltcod -lSDL2 -Wl,-rpath=.

# Define the source files and the target executable
SRCS = $(wildcard src/*.cpp)
TARGET = tuto

# Rule to build the target executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Rule to clean the build (remove the executable)
clean:
	rm -f $(TARGET)

# Rule to rebuild the project
rebuild: clean $(TARGET)
