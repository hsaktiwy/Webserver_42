C = c++
flag = -Wall -Wextra -Werror -std=c++98 -g
src = server.cpp
client = client.cpp
obj_client = $(client:.cpp=.o)
obj = $(src:.cpp=.o)
Name = server
all : $(Name)

$(Name) : $(obj)
	$(C) $(flag) $(obj) -o $@
%.o : %.cpp
		$(C) $(flag) -c $< -o $@
client: $(obj_client)
	$(C) $(flag) $(obj_client) -o $@
# don't forget to delete the cleint src, obj_client, client rule and modifie the clean rule
clean:
		rm -f $(obj) $(client)
fclean: clean
		rm -f $(Name)
re: fclean all

.PHONY: re fclean all clean