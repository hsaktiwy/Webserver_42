C = c++
flag = -Wall -Wextra -Werror -std=c++98 -g
src = server.cpp
obj = $(src:.cpp=.o)
Name = server
all : $(Name)

$(Name) : $(obj)
	$(C) $(flag) $(obj) -o $@
%.o : %.cpp
		$(C) $(flag) -c $< -o $@
clean:
		rm -f $(obj)
fclean: clean
		rm -f $(Name)
re: fclean all

.PHONY: re fclean all clean