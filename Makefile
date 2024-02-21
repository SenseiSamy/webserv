CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
NAME = webserv

RED=\033[0;31m
GREEN=\033[0;32m
NC=\033[0m

all: $(NAME)

# Linking
$(NAME): $(OBJ_FILES)
	@echo "${GREEN}Linking...${NC}"
	@$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "${GREEN}Done. Executable created: $(NAME)${NC}"

# create folder
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Compiling
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(OBJ_DIR)
	@echo "${GREEN}Compiling $<...${NC}"
	@$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c -o $@ $<

clean:
	@echo "${RED}Cleaning...${NC}"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "${RED}Removing executable...${NC}"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

