# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/22 16:36:00 by tde-raev          #+#    #+#              #
#    Updated: 2025/05/21 15:31:57 by tde-raev         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
SRC = src/minishell.c src/lexer_utils.c src/system_utils.c src/string_utils.c src/token_handlers.c src/free_utils.c
OBJ = obj
NAME = minishell
EXEC_OBJS = $(SRC:src/%.c=obj/%.o)

all: $(NAME)

$(NAME): $(EXEC_OBJS)
	$(CC) $(CFLAGS) $(EXEC_OBJS) -o $(NAME) -L./libft -lft -lreadline

# Compile object files
obj/%.o: src/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all
