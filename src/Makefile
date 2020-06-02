 
############################# Makefile ##########################
VGFLAGS = --leak-check=full -s --show-reachable=no --track-origins=yes --keep-stacktraces=alloc-and-free --error-limit=no --num-callers=40 --show-leak-kinds=all

make:
	gcc -g -o smecep main.c

#grind:
#	valgrind $(VGFLAGS) ./$(EXECUTABLE) 1	

fmt:
	@-clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -verbose -i *.c 	