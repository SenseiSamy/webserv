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

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC) -MMD -c $< -o $@

-include $(DEP)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

scan-build: clean
	scan-build make

.PHONY: all clean fclean re scan-build
