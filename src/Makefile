 
############################# Makefile ##########################
VGFLAGS = --leak-check=full -s --show-reachable=yes --track-origins=yes --keep-stacktraces=alloc-and-free --error-limit=no --num-callers=40 --show-leak-kinds=all
EXECUTABLE=argus

clean:
	rm -f argusd
	rm -f argus
	rm -f fifo_client_to_server
	rm -f fifo_server_to_client
	rm -f log.txt
	rm -f log.idx
	pkill argusd || true
	pkill argus || true

make all:
	gcc -o  argusd argusd.c
	gcc -o argus argus.c

	@ if ! [  -a fifo_client_to_server ] ; \
		then \
     mkfifo "fifo_client_to_server" -m666  ; \
		fi;

	@ if ! [  -a fifo_server_to_client ] ; \
		then \
     mkfifo "fifo_server_to_client" -m666   ; \
		fi;


grind:
	valgrind $(VGFLAGS) ./$(EXECUTABLE) 

fmt:
	@-clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -verbose -i *.c 	
