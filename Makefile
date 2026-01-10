NAME =		ircserv
CXX =		c++
CXXFLAGS =	-Wall -Wextra -Werror -std=c++98 -I includes

SRC	= 		main.cpp
OBJDIR = 	obj
OBJ = 		$(SRC:%.cpp=$(OBJDIR)/%.o)

all:		$(NAME)

$(NAME): 	$(OBJ)
			$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re:	fclean all

# bonus: all

.PHONY:	all clean fclean re%
# .PHONY:	all clean fclean re bonus%