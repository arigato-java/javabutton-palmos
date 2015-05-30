.PHONY: clean
CFLAGS=-Wall -Os -I../rsrc -I.

grc-timestamp: javabutton
	m68k-palmos-obj-res javabutton
	touch grc-timestamp

javabutton: javabutton.c
	m68k-palmos-gcc $(CFLAGS) -o javabutton javabutton.c

clean:
	rm -f javabutton *.grc grc-timestamp
