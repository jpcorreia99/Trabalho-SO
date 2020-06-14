 
############################# Makefile ##########################
VGFLAGS = --leak-check=full -s --show-reachable=yes --track-origins=yes --keep-stacktraces=alloc-and-free --error-limit=no --num-callers=40 --show-leak-kinds=all
EXECUTABLE=argus

make clean:
	rm argusd
	rm argus
	rm fifo_client_to_server
	rm fifo_server_to_client
	rm output.txt
	rm log.idx
make all:
	gcc -o  argusd argusd.c
	gcc -o argus argus.c
	mkfifo "fifo_client_to_server" -m666 
	mkfifo "fifo_server_to_client" -m666 

grind:
	valgrind $(VGFLAGS) ./$(EXECUTABLE) 

fmt:
	@-clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -verbose -i *.c 	
