#####################################################################################################################
NAME = webserv
#####################################################################################################################
COLOR_RED		= \e[31m
COLOR_GREEN		= \e[32m
COLOR_ORANGE	= \e[33m
COLOR_BLUE		= \e[34m
COLOR_CYAN		= \e[36m
COLOR_RESET		= \e[0m
#####################################################################################################################
CXX			:= c++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98 -g3 -fdiagnostics-color=always
#####################################################################################################################
VPATH=src/:
OBJ_FOLDER=obj

HEADERS=	Server.hpp
SRC=		main.cpp	\
			Server.cpp
OBJ=$(SRC:%.cpp=$(OBJ_FOLDER)/%.o)
#####################################################################################################################

all: $(NAME)

# Programm compilation rule. It calls to the necessary dependencies rules, so
# 	the files can be compiles
$(NAME): $(OBJ)
	@make -s CLEAN_EXTENSION
	@echo "\n$(COLOR_ORANGE)[ EXE ] Compiling $@...$(COLOR_RESET)"
	@$(CXX) $^ -o $@

# It compiles de .cpp files, listing the current file.
#	· If there is any problem during the compilation, the error will be printed,
# 	the test will be mark as 'KO' and the execution end with the compilation error
# 
# 	· If the compilation finish correctly, the test will be mark as 'OK', and the
# 	execution will continue
$(OBJ_FOLDER)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(OBJ_FOLDER)

	@if [ ! -e $(OBJ_FOLDER)/.cpp ]; then			\
		touch $(OBJ_FOLDER)/.cpp;					\
		echo "$(COLOR_BLUE)[ C++ ] Compiling c++ files ($(COLOR_CYAN)$(CXXFLAGS)$(COLOR_BLUE))$(COLOR_RESET)";		\
	fi


	@echo -n "\t$(COLOR_CYAN)[......]  Compiling $<...$(COLOR_RESET)"
	@RES=$$($(CXX) $(CXXFLAGS) -c $< -o $@ 2> $(OBJ_FOLDER)/.error; echo $$?);	\
	if [ $$RES -ne 0 ]; then													\
		echo "\r\t$(COLOR_RED)[ ERROR ]$(COLOR_RESET)";							\
		tail -n +1 $(OBJ_FOLDER)/.error;										\
		make -s CLEAN_EXTENSION													\
		exit $$RES;																\
	fi

	@echo "\r\t$(COLOR_GREEN)[  OK  ] $(COLOR_RESET)"

# Rule to clean the extension flags that are created during compilation. This flags
# are used to print msgs
CLEAN_EXTENSION:
	@rm -rf $(OBJ_FOLDER)/.cpp $(OBJ_FOLDER)/.error


#####################################################################################################################

# Standard clean rule
clean:
	@rm -rf $(OBJ_FOLDER)

# Standard fclean rule
fclean: clean
	@rm -rf $(NAME)

# Standard recompilation rule
re: fclean all

#####################################################################################################################

# Variables to execute the programm. It can be modified to add the parameters
# that you want to pass to the program.
EXECUTE_WAY	= ./$(NAME)
EXECUTE_ARGS= 8080 10

# Debugger information
DEFAULT_SCRUBBER		= valgrind
DEFAULT_SCRUBBER_FLAGS	= -s --leak-check=full --show-leak-kinds=all --track-fds=yes


# Rule to clean the terminal
clear:
	@clear

# Rules to execute the program. Should be modified if the programm recieves
# any arguments or a pattern
t: run
r: run
run: all clear
	@$(SCRUBBER) $(SCRUBBER_FLAGS) $(EXECUTE_WAY) $(EXECUTE_ARGS)

# Rule to execute
v: valgrind
valgrind: all
valgrind: SCRUBBER=$(DEFAULT_SCRUBBER)
valgrind: SCRUBBER_FLAGS=$(DEFAULT_SCRUBBER_FLAGS)
valgrind: run

#####################################################################################################################

# Rules that won't be treated as files
.PHONY: all clean fclean re r run v valgrind
