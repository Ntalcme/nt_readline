NAME		:= libnt_readline.a
TEST		:= nt_readline_test
CC			:= gcc
CFLAGS		:= -std=gnu99 -Wall -Wextra -Werror -pedantic -Wshadow
CFLAGS		+= -g -MMD -MP -I./include
SAN			?= 
CFLAGS		+= $(SAN)

SRC_DIR		:= src
TEST_DIR	:= test
OBJ_DIR		:= obj
LIB_DIR		:= lib

SRCS		:= $(wildcard $(SRC_DIR)/*.c)
OBJS		:= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_SRCS	:= $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS	:= $(TEST_SRCS:$(TEST_DIR)/%.c=$(OBJ_DIR)/test/%.o)
DEPS		:= $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

MKDIR		:= mkdir -p
RM			:= rm -rf

all: 					$(LIB_DIR)/$(NAME)

debug:
						$(MAKE) re test SAN=-fsanitize=address

$(LIB_DIR)/$(NAME): 	$(OBJS)
						@$(MKDIR) $(@D)
						ar rcs $@ $^

test:					$(TEST)

$(TEST): 				$(TEST_OBJS) $(LIB_DIR)/$(NAME)
						$(CC) $(TEST_OBJS) $(SAN) -L$(LIB_DIR) -lnt_readline -o $@

$(OBJ_DIR)/%.o: 		$(SRC_DIR)/%.c
						@$(MKDIR) $(@D)
						$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/test/%.o: 	$(TEST_DIR)/%.c
						@$(MKDIR) $(@D)
						$(CC) $(CFLAGS) -c $< -o $@

-include 				$(DEPS)

clean:
						$(RM) $(OBJ_DIR)

fclean: 				clean
						$(RM) $(LIB_DIR) $(TEST)

re: 					fclean all

.PHONY: all debug test clean fclean re
