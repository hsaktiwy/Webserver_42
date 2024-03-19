# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/13 12:12:25 by adardour          #+#    #+#              #
#    Updated: 2024/03/19 22:15:12 by aalami           ###   ########.fr        #
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
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o : %.cpp $(HPP)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) 

fclean: clean
	rm -f $(TARGET)
re: clean all
