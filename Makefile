CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -ggdb3

SRC_DIR = src
INC_DIR = inc
OBJ_DIR = .obj

NAME = webserv

# $(shell find src -name "*.cpp")
SRC_FILES = src/main.cpp src/Server/syntax.cpp src/Server/Server.cpp src/Server/display.cpp src/Server/getters.cpp src/Server/parsing.cpp src/Server/socket.cpp src/Response/Response.cpp src/Response/setters.cpp src/Response/display.cpp src/Response/getters.cpp src/Request/Request.cpp src/Request/display.cpp src/Request/getters.cpp src/Cgi/meta_variables.cpp src/Cgi/Cgi.cpp
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
