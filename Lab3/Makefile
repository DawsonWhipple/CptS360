SOURCES=main.c
TARGET=main

all: $(TARGET)

$(TARGET): $(SOURCES)
	gcc -o $@ $^ -m32 -O0 -g -ggdb -Wno-implicit-function-declaration

clean:
	rm -f $(TARGET)