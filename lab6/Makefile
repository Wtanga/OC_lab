all:
	gcc -c -fPIC cookie.c
	gcc -shared -o libcookies.so cookie.o
	gcc -c server.c
	gcc -o client client.c
	gcc -o LiteSH server.o -L. -lcookies -ldl -pthread