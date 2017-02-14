bbclient: driver.o client.o menu.o UDPLib.o
	gcc -pthread -o bbclient driver.o client.o menu.o UDPLib.o
    
driver: driver.c
	gcc driver.c

client.o: client.c client.h
	gcc -c client.c

menu.o: menu.c menu.h
	gcc -c menu.c
    
UDPLib.o: UDPLib.c UDPLib.h
	gcc -c UDPLib.c