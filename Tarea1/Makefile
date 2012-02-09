## 
# Makefile - Tarea 1 - CC31A - 2007 
# Sebastian Gamboa Moyano
## 

CC=gcc
CFLAGS=-pedantic -Wall -ansi -g

default: binarios

binarios: lista.o arbol.o
	@echo Compilados binarios lista.o y arbol.o

eval: lista.o arbol.o eval.o
	@echo 
	@echo Compilando programa principal a $@
	$(CC) $(CFLAGS) $? -o $@
	
clean:
	@echo 
	@echo Limpiando archivos de compilación
	-rm *.o
