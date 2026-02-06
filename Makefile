NAME =		ircserv
CXX =		c++
CXXFLAGS =	-Wall -Wextra -Werror -std=c++98 -I includes

SRC	= 		main.cpp \
			args.cpp \
			Server.cpp \
			Client.cpp \
			Message.cpp \
			Channel.cpp \
			commands/CAP.cpp \
			commands/INVITE.cpp \
			commands/JOIN.cpp \
			commands/KICK.cpp \
			commands/MODE.cpp \
			commands/NICK.cpp \
			commands/PART.cpp \
			commands/PASS.cpp \
			commands/PING.cpp \
			commands/PRIVMSG.cpp \
			commands/QUIT.cpp \
			commands/TOPIC.cpp \
			commands/USER.cpp

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

.PHONY:	all clean fclean re%
