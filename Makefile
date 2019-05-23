OBJ_FILES = build/init.o build/getch.o build/screen.o build/colors.o
CFLAGS = -I include -O3
INSTALL_DIR = ~/.local/

all: lib/libconio.a

build/%.o: src/%.c include/conio2.h include/conio.h
	$(CC) -o $@ -c $< $(CFLAGS)

lib/libconio.a: $(OBJ_FILES)
	ld -r $^ -o build/conio.o
	objcopy -w -L priv_* build/conio.o
	$(AR) -rcs $@ build/conio.o

install: all
	cp include/* $(INSTALL_DIR)/include
	cp lib/* $(INSTALL_DIR)/lib

clean:
	rm -rf build/*
	rm -rf lib/*
