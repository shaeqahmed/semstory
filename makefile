compile: client.c control.c
	gcc client.c -o clientProg
	gcc control.c -o controlProg