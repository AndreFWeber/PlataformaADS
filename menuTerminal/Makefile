############################# Makefile ##########################
all:	man 
	chmod 777 /dev/ttyUSB*

run: man
	./man

.PHONY: all run

man: manager.c manager.h
	gcc -o man manager.c -g -I.

clean:
	rm -rf man
	rm -rf *~
mrproper: clean
	rm -rf man

