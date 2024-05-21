NAME := webserv

CXX := g++
CXXFLAGS := -std=c++98 -Wall -Wextra -Werror -pedantic -ggdb3

SRC_DIR := src
INC_DIR := inc
OBJ_DIR := .obj

SRC := $(shell find $(SRC_DIR) -name "*.cpp")
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEP := $(OBJ:%.o=%.d)

INC := -I$(INC_DIR)

.PHONY: all
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC) -MMD -c $< -o $@

-include $(DEP)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)

.PHONY: fclean
fclean: clean
	rm -f $(NAME)

.PHONY: re
re: fclean all

.PHONY: scan-build
scan-build: clean
	scan-build make

