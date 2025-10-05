CC=clang
CFLAGS = -Wall -Wextra -I./include/
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = keepassxc-tui
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -vf ($OBJ) 
