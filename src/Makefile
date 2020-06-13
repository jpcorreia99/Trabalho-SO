 
############################# Makefile ##########################
VGFLAGS = --leak-check=full -s --show-reachable=yes --track-origins=yes --keep-stacktraces=alloc-and-free --error-limit=no --num-callers=40 --show-leak-kinds=all
EXECUTABLE=argus


make servidor:
	gcc -o  server server.c

make cliente:
	gcc -ggdb3 -o argus argus.c

grind:
	valgrind $(VGFLAGS) ./$(EXECUTABLE) 

fmt:
	@-clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -verbose -i *.c 	
