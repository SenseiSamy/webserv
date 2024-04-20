CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -ggdb3

SRC_DIR = src
INC_DIR = inc
OBJ_DIR = .obj

SRC_FILES =  $(shell find $(SRC_DIR) -name "*.cpp")
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEP_FILES = $(OBJ_FILES:.o=.d)
NAME = webserv

RED=\033[0;31m
GREEN=\033[0;32m
NC=\033[0m
YELLOW=\033[0;33m
BLUE=\033[0;34m

-include $(DEP_FILES)

define compile_template
@printf "$(BLUE)[%3d%%]$(NC) $(YELLOW)Building CXX object $@...$(NC)\n" $1
@mkdir -p $(@D)
@$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c -o $@ $< -MMD
endef

all: $(NAME)

$(NAME): $(OBJ_FILES)
	@printf "$(GREEN)[100%%] Linking CXX executable $(NAME)...$(NC)\n"
	@$(CXX) $(CXXFLAGS) -o $@ $^
	@printf "$(GREEN)-- Built target $(NAME)$(NC)\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(call compile_template,$(shell echo $$(($(MAKEINDEX)*100/$(words $(OBJ_FILES))))))
	@$(eval MAKEINDEX=$(shell echo $$(($(MAKEINDEX)+1))))

clean:
	@printf "$(RED)Cleaning...$(NC)\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "$(RED)Removing executable $(NAME)...$(NC)\n"
	@rm -f $(NAME)

re: fclean all

scan-build: clean
	@scan-build make

.PHONY: all clean fclean re scan-build update_makeindex

MAKEINDEX = 0
$(OBJ_FILES): | update_makeindex
update_makeindex:
	@$(eval MAKEINDEX=$(shell echo $$(($(MAKEINDEX)+1))))
