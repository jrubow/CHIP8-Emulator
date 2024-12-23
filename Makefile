CC = gcc
CFLAGS = -I./SDL2/include/
LDFLAGS = -L./SDL2/lib -lmingw32 -lSDL2main -lSDL2

TARGET = emulator.exe

SOURCES = emulator.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
 			
