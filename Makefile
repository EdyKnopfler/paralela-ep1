CFLAGS = -Wall -Wextra -ansi -pedantic -O3
pgrep: pgrep.o processamento.o bufsaida.o
	${CC} -o $@ $^ -lpthread
