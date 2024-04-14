CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -ggdb3

SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

SRC_FILES =  $(SRC_DIR)/main.cpp $(SRC_DIR)/Logger/Logger.cpp $(SRC_DIR)/cgi/cgi.cpp $(SRC_DIR)/cgi/meta_variables.cpp $(SRC_DIR)/Server.cpp
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
NAME = webserv

RED=\033[0;31m
GREEN=\033[0;32m
NC=\033[0m

all: $(NAME)

$(NAME): $(OBJ_FILES)
	@printf "${GREEN}Linking...${NC}\n"
	@$(CXX) $(CXXFLAGS) -o $@ $^
	@printf "${GREEN}Done. Executable created: $(NAME)${NC}\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@printf "${GREEN}Compiling $<...${NC}\n"
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c -o $@ $<

clean:
	@printf "${RED}Cleaning...${NC}\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "${RED}Removing executable...${NC}\n"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
