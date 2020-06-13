 
############################# Makefile ##########################
VGFLAGS = --leak-check=full -s --show-reachable=no --track-origins=yes --keep-stacktraces=alloc-and-free --error-limit=no --num-callers=40 --show-leak-kinds=all
EXECUTABLE=smecep


make servidor:
	gcc -o  server server.c

make cliente:
	gcc -o argus argus.c

grind:
	valgrind $(VGFLAGS) ./$(EXECUTABLE) 

fmt:
	@-clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -verbose -i *.c 	
