CC:= gcc
NM:= client_app
FILE:= client_tcp.o client_tcp_function.o

$(NM):$(FILE)
	$(CC) $(FILE) -o $(NM)

%.o:%.c
	$(CC) -c $^ -o $@

clean:
	rm *.o 
