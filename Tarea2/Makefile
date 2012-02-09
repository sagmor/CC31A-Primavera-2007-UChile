## 
# Makefile - Tarea 2 - CC31A - 2007 
# Sebastian Gamboa Moyano
## 

CC=gcc
CFLAGS=-pedantic -Wall -ansi

default: eval

eval: lista.o arbol.o eval.o
	@echo 
	@echo Compilando programa principal a $@
	$(CC) $(CFLAGS) lista.o arbol.o eval.o -o $@
	
clean:
	@echo 
	@echo Limpiando archivos de compilación
	-rm *.o
