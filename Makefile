CFLAGS = -I include

INSTALL_DIR = ~/.local/

all: lib/libconio.a

build/%.o: src/%.c include/conio2.h include/conio.h
	$(CC) -o $@ -c $< $(CFLAGS)

lib/libconio.a: build/conio.o
	$(AR) -rs $@ $^

install: all
	mkdir -p $(INSTALL_DIR)/include
	mkdir -p $(INSTALL_DIR)/lib
	cp include/* $(INSTALL_DIR)/include
	cp lib/* $(INSTALL_DIR)/lib
