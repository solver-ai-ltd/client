# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Libraries
LIBS = -lcurl
#lcurl installation: sudo apt-get install libcurl4-openssl-dev

# Include paths for header files
INCLUDES = -I.

# Common source files
COMMON_SRCS = SolverAiClientSetup.cpp SolverAiClientCompute.cpp SolverAiComputeInput.cpp SolverAiComputeResults.cpp

# Object files for common source files
COMMON_OBJS = $(COMMON_SRCS:.cpp=.o)

# Example source files
EXAMPLE_SRCS := $(wildcard example_*.cpp) delete_everything.cpp

# Executable names for example source files
EXAMPLE_EXECS = $(EXAMPLE_SRCS:.cpp=)

# Will first clean then compile all example_*.cpp codes
all: clean $(EXAMPLE_EXECS)

$(EXAMPLE_EXECS): $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(COMMON_OBJS) $@.cpp $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) $(COMMON_OBJS) $(EXAMPLE_OBJS) $(EXAMPLE_EXECS)
