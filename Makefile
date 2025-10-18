CXX = g++
CC  = $(CXX)

CXXFLAGS = -std=c++20
CXXFLAGS += -pedantic -Wall -I./include # including the "include" directory where the headers are stored.
CXXFLAGS += -g                  # debugging
# CXXFLAGS += -O2                 # optimizations?
INC_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
EXE = run
SRC = $(wildcard $(SRC_DIR)/*.cpp) # creating a list of all the path of the source files.
OBJS = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) # creating the list of the paths for the object files that need to be created.
DEPS = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.d)

.PHONY: all

all:$(EXE)

$(EXE): $(OBJS) # create the binary with the entire list of the objects and create the object folder if it doesn't exist
	g++ -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) # create an object file out of each source file.
	$(CXX) -c $(CXXFLAGS) -MP -MMD -o $@ $<
# -MP, each dependency has a phony target so this should avoid random errors when removing and adding headers.
#  # -MMD generates a dependency file for each object, including only our headers and not the system headers (-MD includes system headers)

$(OBJ_DIR):
	mkdir -p $@
# making the object directory but it also contains the respective dependency files.

clean:
	rm -f $(OBJS) $(DEPS) $(EXE)
	rmdir $(OBJ_DIR)

format:
	@find . -regex '.*\.\(cpp\|hpp\|cu\|cuh\|c\|h\)' -exec clang-format -style=file -i {} \;
