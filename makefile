#makefile

miniShell: miniShell.c
	gcc -o miniShell miniShell.c
clean:
	rm miniShell
