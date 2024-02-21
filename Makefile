CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

SRC_FILES =  $(SRC_DIR)/main.cpp $(SRC_DIR)/logger/logger.cpp
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
NAME = webserv

RED=\033[0;31m
GREEN=\033[0;32m
NC=\033[0m

all: $(NAME)

$(NAME): $(OBJ_FILES)
	@echo "${GREEN}Linking...${NC}"
	@$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "${GREEN}Done. Executable created: $(NAME)${NC}"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "${GREEN}Compiling $<...${NC}"
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c -o $@ $<

clean:
	@echo "${RED}Cleaning...${NC}"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "${RED}Removing executable...${NC}"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
