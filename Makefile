FLAGS = -std=gnu11 -Werror -Wall
all: defrag
defrag:	defrag.c 
	cc ${FLAGS} -o defrag defrag.c -g -lpthread