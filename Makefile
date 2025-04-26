NAME = ircserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp sources/Authenti.cpp sources/Channel.cpp sources/Client.cpp sources/Server.cpp \
	commands/INVITE.cpp commands/JOIN.cpp commands/KICK.cpp commands/MODE.cpp commands/PART.cpp commands/PRIVMSG.cpp commands/QUIT.cpp commands/TOPIC.cpp 

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp includes/Server.hpp includes/Client.hpp includes/Channel.hpp includes/replies.hpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re