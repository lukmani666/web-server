CC = gcc
CFLAGS = -Wall -Iinclude
TARGET = web_server
SRC_DIR = src
OBJ_DIR = obj

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/server.c $(SRC_DIR)/request_handler.c \
	$(SRC_DIR)/response.c $(SRC_DIR)/utils.c $(SRC_DIR)/mime_types.c

OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)