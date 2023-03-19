CC=gcc
CFLAGS+=-Wall -Wextra -Werror 
OBJ+=src/my_tar.o src/utils.o src/header.o

TARGET=my_tar

all: $(TARGET)

$(TARGET): $(OBJ) $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

.PHONY: clean
clean:
	rm -f src/*.o *.tar $(TARGET)
