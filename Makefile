# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/13 12:12:25 by adardour          #+#    #+#              #
#    Updated: 2024/02/10 18:44:26 by hsaktiwy         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++ -std=c++98
CFLAGS = -Wall -Wextra -Werror -fsanitize=address
SANI = #-fsanitize=address
SRC = Client.cpp            ft_memset.cpp         http.server.cpp       main.cpp              proccess_tokens.cpp   response.cpp          start_serving.cpp \
blockworker.cpp       handle_errors.cpp     init_worker_block.cpp mime_types.cpp        request.cpp           start_listening.cpp   tools.cpp cgi/cgi.cpp    cgi/cgiEnv.cpp
HPP = Client.hpp      http.server.hpp request.hpp     response.hpp cgi/cgi.hpp

OBJ = $(SRC:.cpp=.o)

TARGET = webserv

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(SANI) $(OBJ) -o $@

%.o : %.cpp Client.hpp      http.server.hpp request.hpp     response.hpp cgi/cgi.hpp
	$(CC) $(SANI) -c $< -o $@

clean:
	rm -f $(OBJ) 

fclean: clean
	rm -f $(TARGET)
re: clean all
