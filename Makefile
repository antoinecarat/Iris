CC := gcc # This is the main compiler
CFLAGS := -std=gnu99 -Wall -pthread -g #-Wwrite-strings

SRCDIR := src
BUILDDIR := build
TARGET-CLIENT := bin/iris
TARGET-SERVER := bin/iris-server

HEADERS := $(shell find src -name *.h)
SOURCES := $(shell find src -name *.c)
OBJECTS := build/fileManager.o build/networkManager.o
OBJECTS-CLIENT := build/iris.o
OBJECTS-SERVER := build/iris-server.o

all : $(TARGET-CLIENT) $(TARGET-SERVER)

doc: $(HEADERS)
	rm -rf doc/
	doxygen Doxyfile

clean:
	rm -rf build/ bin/ doc/ iris/ iris-server/

$(TARGET-CLIENT): $(OBJECTS) $(OBJECTS-CLIENT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $(TARGET-CLIENT)

$(TARGET-SERVER): $(OBJECTS) $(OBJECTS-SERVER)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $(TARGET-SERVER)

$(OBJECTS): $(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECTS-CLIENT): $(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECTS-SERVER): $(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean