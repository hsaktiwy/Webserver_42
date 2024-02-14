# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aalami < aalami@student.1337.ma>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/13 12:12:25 by adardour          #+#    #+#              #
#    Updated: 2024/02/14 22:06:22 by aalami           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++ -std=c++98
CFLAGS = -Wall -Wextra -Werror -fsanitize=address
SANI = #-fsanitize=address
SRC = Client.cpp            ft_memset.cpp         http.server.cpp       main.cpp              proccess_tokens.cpp   response.cpp          start_serving.cpp \
blockworker.cpp       handle_errors.cpp     init_worker_block.cpp mime_types.cpp        request.cpp           start_listening.cpp   tools.cpp cgi/cgi.cpp    cgi/cgiResponse.cpp
HPP = Client.hpp      http.server.hpp request.hpp     response.hpp cgi/cgi.hpp cgi/cgiResponse.hpp

OBJ = $(SRC:.cpp=.o)

TARGET = webserv

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(SANI) $(OBJ) -o $@

%.o : %.cpp Client.hpp      http.server.hpp request.hpp     response.hpp cgi/cgi.hpp cgi/cgiResponse.hpp
	$(CC) $(SANI) -c $< -o $@

clean:
	rm -f $(OBJ) 

fclean: clean
	rm -f $(TARGET)
re: clean all
