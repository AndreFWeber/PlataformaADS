############################# Makefile ##########################
all:	man 
	sudo chmod 777 /dev/ttyUSB*
	./man

run: man
	./man

.PHONY: all run

man: manager.c manager.h
	gcc -o man manager.c xmlParse.c -I/usr/include/libxml2 -L/usr/lib -lxml2 -lz -lpthread -lm -g

clean:
	rm -rf man
	rm -rf *~
mrproper: clean
	rm -rf man

