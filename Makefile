CC = clang
CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude

TARGET = catsh

SRC_DIR = src
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/tokenizer.c \
       $(SRC_DIR)/catsh_conf.c \
       $(SRC_DIR)/charvec.c \
       $(SRC_DIR)/terminal.c \
       $(SRC_DIR)/env.c  \
       $(SRC_DIR)/journal.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean allls
