CC:= gcc
NM:= server_tcp_app
FILE:= server_tcp.o server_tcp_function.o
LINK:= -lsqlite3
$(NM):$(FILE)
	$(CC) $(FILE) -o $(NM) $(LINK) 

%.o:%.c
	$(CC) -c $^ -o $@ 

clean:
	rm *.o 
