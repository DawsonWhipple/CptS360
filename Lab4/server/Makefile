SOURCES=main.c server.c commands.c
TARGET=main

all: $(TARGET)

$(TARGET): $(SOURCES)
	gcc -o $@ $^ -m32

clean:
	rm -f $(TARGET)