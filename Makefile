CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -ggdb3

SRC_DIR = src
INC_DIR = inc
OBJ_DIR = .obj

NAME = webserv

SRC_FILES = $(shell find src -name "*.cpp")
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
DEP_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.d, $(SRC_FILES))

all: $(NAME)

$(NAME): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -c -o $@ $<

-include $(DEP_FILES)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

scan-build: clean
	scan-build make

.PHONY: all clean fclean re scan-build
