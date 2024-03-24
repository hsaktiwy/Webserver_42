# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adardour <adardour@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/13 12:12:25 by adardour          #+#    #+#              #
#    Updated: 2024/03/23 19:55:56 by adardour         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++ -std=c++98

CFLAGS = -Wall -Werror -Wextra

SRC = srcs/Client.cpp       srcs/http.server.cpp       srcs/main.cpp              srcs/proccess_tokens.cpp   srcs/response.cpp \
srcs/blockworker.cpp       srcs/handle_errors.cpp     srcs/init_worker_block.cpp srcs/mime_types.cpp        srcs/request.cpp           srcs/start_listening.cpp   srcs/tools.cpp srcs/cgi/cgi.cpp    srcs/cgi/cgiResponse.cpp srcs/timer.cpp

INC = ./includes
HPP = $(INC)/Client.hpp      $(INC)/http.server.hpp $(INC)/request.hpp     $(INC)/response.hpp $(INC)/cgi.hpp $(INC)/cgiResponse.hpp \


OBJ = $(SRC:.cpp=.o)

TARGET = webserv

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o : %.cpp $(HPP)
	$(CC)  -c $< -o $@

clean:
	rm -f $(OBJ) 

fclean: clean
	rm -f $(TARGET)
re: clean all
