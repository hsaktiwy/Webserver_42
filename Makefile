# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adardour <adardour@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/13 12:12:25 by adardour          #+#    #+#              #
#    Updated: 2024/03/12 22:27:57 by adardour         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++ -std=c++98

CFLAGS = -Wall -Wextra -Werror -fsanitize=address

SANI = #-fsanitize=address

SRC =      proccess_tokens.cpp  start_listening.cpp init_worker_block.cpp blockworker.cpp handle_errors.cpp  Client.cpp            ft_memset.cpp         http.server.cpp       main.cpp           response.cpp    \
            mime_types.cpp   request.cpp  timer.cpp   tools.cpp cgi/cgi.cpp   cgi/cgiResponse.cpp  

HPP = Client.hpp      http.server.hpp request.hpp     response.hpp cgi/cgi.hpp cgi/cgiResponse.hpp \


OBJ = $(SRC:.cpp=.o)

TARGET = webserv

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(SANI) $(OBJ) -o $@

%.o : %.cpp Client.hpp      http.server.hpp request.hpp     response.hpp cgi/cgi.hpp cgi/cgiResponse.hpp
	$(CC) $(CFLAGS) $(SANI) -c $< -o $@

clean:
	rm -f $(OBJ) 

fclean: clean
	rm -f $(TARGET)
re: clean all
