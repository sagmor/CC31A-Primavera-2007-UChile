## 
# Makefile - Tarea 3 - CC31A - 2007 
# Sebastian Gamboa Moyano
## 

APP = gusano
VERSION = tarea3

# Build Mode:
#		DEBUG: Development version.
#		RELEASE: Final Delivery Version.
MODE = RELEASE

# Main Macros
SHELL = /bin/bash
CC=gcc
OBJ = $(addsuffix .o, $(basename $(SRC)))
HEADERS = $(addsuffix .h, $(basename $(SRC)))
MAIN_FILES = lista.c arbol.c eval.c listas.c wish.c gusano.c
DEBUG_FILES = debug.c
MAIN_FLAGS = -pedantic -Wall -ansi
DEBUG_FLAGS = -g -DDEBUGING
PACK_FILES = Makefile $(SRC) $(HEADERS) camino.txt gus_prog debug.h debug.c inteligente die

ifeq "$(MODE)" "DEBUG"
CFLAGS = $(DEBUG_FLAGS) $(MAIN_FLAGS) 
SRC = $(DEBUG_FILES) $(MAIN_FILES)
else
ifeq "$(MODE)" "RELEASE"
CFLAGS = $(MAIN_FLAGS)
SRC = $(MAIN_FILES)
endif
endif

default: build

build: start $(APP) end

$(APP): $(OBJ)
	@echo
	@echo Building Main application to $@:
	$(CC) $(CFLAGS) $(OBJ) -o $@
	

start:
	@echo Building Application: \"$(APP)\"
	@echo Version: $(VERSION)
	@echo Mode: $(MODE)
	@echo
	@echo Starting Build Process:

end:
	@echo Build process ended!
	@if [ -f README ]; then echo""; cat README; else echo ""; fi
	
gusano_orig: listas.o wish.o gusano_orig.o
	$(CC) $(CFLAGS) listas.o wish.o gusano_orig.o -o $@
	
pack: 
	@echo Packing $(APP) source files to $(APP)-$(VERSION).tgz
	tar czf $(APP)-$(VERSION).tgz $(PACK_FILES)
	
debug:
	$(MAKE) MODE=DEBUG
	
release:
	$(MAKE) MODE=RELEASE
	
clean:
	@echo Cleaning Build Files
	@rm -f *.o

depend:
	makedepend -Y $(SRC)