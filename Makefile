# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adardour <adardour@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/13 12:12:25 by adardour          #+#    #+#              #
#    Updated: 2024/03/07 12:56:15 by adardour         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++ -std=c++98

CFLAGS = -Wall -Wextra -Werror -fsanitize=address

SANI = #-fsanitize=address

SRC = Client.cpp            ft_memset.cpp         http.server.cpp       main.cpp              proccess_tokens.cpp   response.cpp          start_serving.cpp \
            mime_types.cpp   request.cpp  timer.cpp   tools.cpp cgi/cgi.cpp  start_listening.cpp  cgi/cgiResponse.cpp  handle_errors.cpp blockworker.cpp init_worker_block.cpp

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
