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
SRC = src/minishell.c src/lexer_utils.c src/system_utils.c src/string_utils.c \
      src/token_handlers.c src/token_utils.c src/free_utils.c src/parser.c \
      src/parser_utils.c src/redirections.c src/builtin.c src/builtin2.c \
      src/builtin_export_utils.c \
      src/external_exec.c src/executor.c src/utils.c src/debug_utils.c \
      src/pipe_handler.c src/pipe_handler_utils.c src/env_expansion.c \
      src/quote_handler.c src/quote_handler_utils.c src/signal_handler.c \
      src/pipe_utils.c
OBJ = obj
NAME = minishell
LIBFT = libft/libft.a
EXEC_OBJS = $(SRC:src/%.c=obj/%.o)

all: $(LIBFT) $(NAME)

$(LIBFT):
	$(MAKE) -C libft

$(NAME): $(EXEC_OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(EXEC_OBJS) -o $(NAME) -L./libft -lft -lreadline

# Compile object files
obj/%.o: src/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -rf $(OBJ)
	$(MAKE) -C libft clean

fclean: clean
	rm -rf $(NAME)
	$(MAKE) -C libft fclean

re: fclean all

.PHONY: all clean fclean re
