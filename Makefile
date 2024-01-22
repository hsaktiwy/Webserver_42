# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hsaktiwy <hsaktiwy@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/13 12:12:25 by adardour          #+#    #+#              #
#    Updated: 2024/01/22 17:21:41 by hsaktiwy         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++ #-std=c++98
CFLAGS = -Wall -Wextra -Werror

SRC = $(wildcard *.cpp)
HPP = $(wildcard *.hpp)

OBJ = $(SRC:.cpp=.o)

TARGET = webserv

all: $(TARGET)

$(TARGET): $(OBJ)  $(HPP)
	$(CC) $(OBJ) -o $@

.cpp.o: $(HPP)
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJ) 

fclean: clean
	rm -f $(TARGET)
re: clean all
