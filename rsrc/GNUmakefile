.PHONY: clean

rsrc-timestamp: jb_rscid.h javabutton.rcp
	pilrc -Fj -noEllipsis javabutton.rcp
	touch rsrc-timestamp

clean:
	rm -f *.bin rsrc-timestamp
